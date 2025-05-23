#include "core.h"

#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QStyleFactory>
#include <utility>
#include "window/mainwindow/mainwindowundo.h"
#ifdef Q_OS_WINDOWS
#include <QtPlatformHeaders/QWindowsWindowFunctions>
#endif

#include "audio/audiomanager.h"
#include "cli/clitask/clitaskdialog.h"
#include "codec/conformmanager.h"
#include "common/filefunctions.h"
#include "common/xmlutils.h"
#include "config/config.h"
#include "dialog/about/about.h"
#include "dialog/autorecovery/autorecoverydialog.h"
#include "dialog/export/export.h"
#include "dialog/footagerelink/footagerelinkdialog.h"
#ifdef USE_OTIO
#include "dialog/otioproperties/otiopropertiesdialog.h"
#endif
#include "dialog/preferences/preferences.h"
#include "dialog/projectproperties/projectproperties.h"
#include "dialog/sequence/sequence.h"
#include "dialog/task/task.h"
#include "node/color/colormanager/colormanager.h"
#include "node/factory.h"
#include "node/nodeundo.h"
#include "node/project/serializer/serializer.h"
#include "panel/panelmanager.h"
#include "panel/project/project.h"
#include "panel/viewer/viewer.h"
#include "render/diskmanager.h"
#include "render/framemanager.h"
#include "render/rendermanager.h"
#ifdef USE_OTIO
#include "task/project/loadotio/loadotio.h"
#include "task/project/saveotio/saveotio.h"
#endif
#include "task/project/import/import.h"
#include "task/project/import/importerrordialog.h"
#include "task/project/load/load.h"
#include "task/project/save/save.h"
#include "task/taskmanager.h"
#include "ui/style/style.h"
#include "undo/undostack.h"
#include "widget/menu/menushared.h"
#include "widget/taskview/taskviewitem.h"
#include "widget/viewer/viewer.h"
#include "window/mainwindow/mainstatusbar.h"
#include "window/mainwindow/mainwindow.h"

namespace olive {

Core* Core::instance_ = nullptr;

Core::Core(CoreParams params)
    : main_window_(nullptr),
      open_project_(nullptr),
      tool_(Tool::kPointer),
      addable_object_(Tool::kAddableEmpty),
      snapping_(true),
      core_params_(std::move(params)),
      magic_(false),
      pixel_sampling_users_(0),
      shown_cache_full_warning_(false) {
  // Store reference to this object, making the assumption that Core will only ever be made in
  // main(). This will obviously break if not.
  instance_ = this;

  translator_ = new QTranslator(this);
}

Core* Core::instance() { return instance_; }

void Core::DeclareTypesForQt() {
  qRegisterMetaType<olive::core::rational>();
  qRegisterMetaType<NodeValue>();
  qRegisterMetaType<NodeValueTable>();
  qRegisterMetaType<NodeValueDatabase>();
  qRegisterMetaType<FramePtr>();
  qRegisterMetaType<SampleBuffer>();
  qRegisterMetaType<AudioParams>();
  qRegisterMetaType<NodeKeyframe::Type>();
  qRegisterMetaType<Decoder::RetrieveState>();
  qRegisterMetaType<olive::core::TimeRange>();
  qRegisterMetaType<olive::core::Color>();
  qRegisterMetaType<olive::AudioVisualWaveform>();
  qRegisterMetaType<olive::VideoParams>();
  qRegisterMetaType<olive::VideoParams::Interlacing>();
  qRegisterMetaType<olive::MainWindowLayoutInfo>();
  qRegisterMetaType<olive::RenderTicketPtr>();
}

void Core::Start() {
  // Load application config
  Config::Load();

  // Set locale based on either startup arg, config, or auto-detect
  SetStartupLocale();

  // Declare custom types for Qt signal/slot system
  DeclareTypesForQt();

  // Set up node factory/library
  NodeFactory::Initialize();

  // Set up color manager's default config
  ColorManager::SetUpDefaultConfig();

  // Initialize task manager
  TaskManager::CreateInstance();

  // Initialize ConformManager
  ConformManager::CreateInstance();

  // Initialize RenderManager
  RenderManager::CreateInstance();

  // Initialize FrameManager
  FrameManager::CreateInstance();

  // Initialize project serializers
  ProjectSerializer::Initialize();

  //
  // Start application
  //

  qInfo() << "Using Qt version:" << qVersion();

  switch (core_params_.run_mode()) {
    case CoreParams::kRunNormal:
      // Start GUI
      StartGUI(core_params_.fullscreen());

      // If we have a startup
      QMetaObject::invokeMethod(this, "OpenStartupProject", Qt::QueuedConnection);
      break;
    case CoreParams::kHeadlessExport:
      qInfo() << "Headless export is not fully implemented yet";
      break;
    case CoreParams::kHeadlessPreCache:
      qInfo() << "Headless pre-cache is not fully implemented yet";
      break;
  }

  // Manual crash triggering
  if (core_params_.crash_on_startup()) {
    const int interval = 5000;
    qInfo() << "Manual crash was triggered. Application will crash in" << interval << "ms";
    auto* crash_timer = new QTimer(this);
    crash_timer->setInterval(interval);
    connect(crash_timer, &QTimer::timeout, this, [] { abort(); });
    crash_timer->start();
  }
}

void Core::Stop() {
  // Assume all projects have closed gracefully and no auto-recovery is necessary
  autorecovered_projects_.clear();
  SaveUnrecoveredList();

  // Save Config
  Config::Save();

  ProjectSerializer::Destroy();

  ConformManager::DestroyInstance();

  FrameManager::DestroyInstance();

  RenderManager::DestroyInstance();

  MenuShared::DestroyInstance();

  TaskManager::DestroyInstance();

  PanelManager::DestroyInstance();

  AudioManager::DestroyInstance();

  DiskManager::DestroyInstance();

  NodeFactory::Destroy();

  delete main_window_;
  main_window_ = nullptr;
}

MainWindow* Core::main_window() { return main_window_; }

UndoStack* Core::undo_stack() { return &undo_stack_; }

void Core::ImportFiles(const QStringList& urls, Folder* parent) {
  if (urls.isEmpty()) {
    QMessageBox::critical(main_window_, tr("Import error"), tr("Nothing to import"));
    return;
  }

  auto* pim = new ProjectImportTask(parent, urls);

  if (!pim->GetFileCount()) {
    // No files to import
    delete pim;
    return;
  }

  auto* task_dialog = new TaskDialog(pim, tr("Importing..."), main_window());

  connect(task_dialog, &TaskDialog::TaskSucceeded, this, &Core::ImportTaskComplete);

  task_dialog->open();
}

const Tool::Item& Core::tool() const { return tool_; }

const Tool::AddableObject& Core::GetSelectedAddableObject() const { return addable_object_; }

const QString& Core::GetSelectedTransition() const { return selected_transition_; }

void Core::SetSelectedAddableObject(const Tool::AddableObject& obj) {
  addable_object_ = obj;
  emit AddableObjectChanged(addable_object_);
}

void Core::SetSelectedTransitionObject(const QString& obj) { selected_transition_ = obj; }

void Core::ClearOpenRecentList() {
  recent_projects_.clear();
  SaveRecentProjectsList();
  emit OpenRecentListChanged();
}

void Core::CreateNewProject() {
  // If we already have an empty/new project, switch to it
  if (CloseProject(false)) {
    auto* p = new Project();
    p->Initialize();
    AddOpenProject(p);
  }
}

const bool& Core::snapping() const { return snapping_; }

const QStringList& Core::GetRecentProjects() const { return recent_projects_; }

void Core::SetTool(const Tool::Item& tool) {
  tool_ = tool;

  emit ToolChanged(tool_);
}

void Core::SetSnapping(const bool& b) {
  snapping_ = b;

  emit SnappingChanged(snapping_);
}

void Core::DialogAboutShow() {
  AboutDialog a(false, main_window_);
  a.exec();
}

void Core::DialogImportShow() {
  // Open dialog for user to select files
  QStringList files = QFileDialog::getOpenFileNames(main_window_, tr("Import footage..."));

  // Check if the user actually selected files to import
  if (!files.isEmpty()) {
    // Locate the most recently focused Project panel (assume that's the panel the user wants to import into)
    auto* active_project_panel = PanelManager::instance()->MostRecentlyFocused<ProjectPanel>();
    Project* active_project;

    if (active_project_panel == nullptr                                      // Check that we found a Project panel
        || (active_project = active_project_panel->project()) == nullptr) {  // and that we could find an active Project
      QMessageBox::critical(main_window_, tr("Failed to import footage"), tr("Failed to find active Project panel"));
      return;
    }

    // Get the selected folder in this panel
    Folder* folder = active_project_panel->GetSelectedFolder();

    ImportFiles(files, folder);
  }
}

void Core::DialogPreferencesShow() {
  PreferencesDialog pd(main_window_);
  pd.exec();
}

void Core::DialogProjectPropertiesShow() {
  Project* proj = GetActiveProject();

  if (proj) {
    ProjectPropertiesDialog ppd(proj, main_window_);
    ppd.exec();
  } else {
    QMessageBox::critical(main_window_, tr("No Active Project"),
                          tr("No project is currently open to set the properties for"), QMessageBox::Ok);
  }
}

void Core::DialogExportShow() {
  if (ViewerOutput* viewer = GetSequenceToExport()) {
    OpenExportDialogForViewer(viewer, false);
  }
}

#ifdef USE_OTIO
bool Core::DialogImportOTIOShow(const QList<Sequence*>& sequences) const {
  Project* active_project = GetActiveProject();
  OTIOPropertiesDialog opd(sequences, active_project);
  return opd.exec() == QDialog::Accepted;
}
#endif

void Core::CreateNewFolder() {
  // Locate the most recently focused Project panel (assume that's the panel the user wants to import into)
  auto* active_project_panel = PanelManager::instance()->MostRecentlyFocused<ProjectPanel>();
  Project* active_project;

  if (active_project_panel == nullptr                                      // Check that we found a Project panel
      || (active_project = active_project_panel->project()) == nullptr) {  // and that we could find an active Project
    QMessageBox::critical(main_window_, tr("Failed to create new folder"), tr("Failed to find active project"));
    return;
  }

  // Get the selected folder in this panel
  Folder* folder = active_project_panel->GetSelectedFolder();

  // Create new folder
  auto* new_folder = new Folder();

  // Set a default name
  new_folder->SetLabel(tr("New Folder"));

  // Create an undoable command
  auto* command = new MultiUndoCommand();

  command->add_child(new NodeAddCommand(active_project, new_folder));
  command->add_child(new FolderAddChild(folder, new_folder));

  Core::instance()->undo_stack()->push(command, tr("Created New Folder"));

  // Trigger an automatic rename so users can enter the folder name
  active_project_panel->Edit(new_folder);
}

void Core::CreateNewSequence() {
  Project* active_project = GetActiveProject();

  if (!active_project) {
    QMessageBox::critical(main_window_, tr("Failed to create new sequence"), tr("Failed to find active project"));
    return;
  }

  // Create new sequence
  Sequence* new_sequence = CreateNewSequenceForProject(active_project);

  SequenceDialog sd(new_sequence, SequenceDialog::kNew, main_window_);

  // Make sure SequenceDialog doesn't make an undo command for editing the sequence, since we make an undo command for
  // adding it later on
  sd.SetUndoable(false);

  if (sd.exec() == QDialog::Accepted) {
    // Create an undoable command
    auto* command = new MultiUndoCommand();

    command->add_child(new NodeAddCommand(active_project, new_sequence));
    command->add_child(new FolderAddChild(GetSelectedFolderInActiveProject(), new_sequence));
    command->add_child(new NodeSetPositionCommand(new_sequence, new_sequence, Node::Position()));
    command->add_child(new OpenSequenceCommand(new_sequence));

    // Create and connect default nodes to new sequence
    new_sequence->add_default_nodes(command);

    Core::instance()->undo_stack()->push(command, tr("Created New Sequence"));

  } else {
    // If the dialog was accepted, ownership goes to the AddItemCommand. But if we get here, just delete
    delete new_sequence;
  }
}

void Core::AddOpenProject(Project* p, bool add_to_recents) {
  // Ensure project is not open at the moment
  if (open_project_ == p) {
    return;
  }

  // If we currently have an empty project, close it first
  if (open_project_) {
    CloseProject(false);
  }

  SetActiveProject(p);

  if (!p->filename().isEmpty() && add_to_recents) {
    PushRecentlyOpenedProject(p->filename());
  }
}

bool Core::AddOpenProjectFromTask(Task* task, bool add_to_recents) {
  auto* load_task = dynamic_cast<ProjectLoadBaseTask*>(task);

  if (!load_task->IsCancelled()) {
    Project* project = load_task->GetLoadedProject();

    if (ValidateFootageInLoadedProject(project, project->GetSavedURL())) {
      AddOpenProject(project, add_to_recents);
      main_window_->LoadLayout(load_task->GetLoadedLayout());

      return true;
    } else {
      delete project;
      CreateNewProject();
    }
  }

  return false;
}

void Core::SetActiveProject(Project* p) {
  if (open_project_) {
    disconnect(open_project_, &Project::ModifiedChanged, this, &Core::ProjectWasModified);
  }

  open_project_ = p;
  RenderManager::instance()->SetProject(p);
  main_window_->SetProject(p);

  if (open_project_) {
    connect(open_project_, &Project::ModifiedChanged, this, &Core::ProjectWasModified);
  }
}

void Core::ImportTaskComplete(Task* task) {
  auto* import_task = dynamic_cast<ProjectImportTask*>(task);

  MultiUndoCommand* command = import_task->GetCommand();

  foreach (Footage* f, import_task->GetImportedFootage()) {
    // Look for multi-layer images
    if (f->GetAudioStreamCount() == 0 && f->GetVideoStreamCount() > 1) {
      bool all_stills = true;

      for (int i = 0; i < f->GetVideoStreamCount(); i++) {
        const VideoParams& vs = f->GetVideoParams(i);
        if (!(vs.video_type() == VideoParams::kVideoTypeStill && vs.enabled() == (i == 0))) {
          all_stills = false;
        }
      }

      if (all_stills) {
        QMessageBox d(main_window());

        d.setIcon(QMessageBox::Question);
        d.setWindowTitle(tr("Multi-Layer Image"));
        d.setText(tr("The file '%1' has multiple layers. Would you like these layers to be "
                     "separated across multiple tracks or merged into a single image?")
                      .arg(f->filename()));

        auto multi_btn = d.addButton(tr("Multiple Layers"), QMessageBox::YesRole);
        auto single_btn = d.addButton(tr("Single Layer"), QMessageBox::NoRole);
        auto cancel_btn = d.addButton(QMessageBox::Cancel);

        d.exec();

        if (d.clickedButton() == multi_btn) {
          for (int i = 0; i < f->GetVideoStreamCount(); i++) {
            VideoParams vs = f->GetVideoParams(i);
            vs.set_enabled(!vs.enabled());
            f->SetVideoParams(vs, i);
          }
        } else if (d.clickedButton() == single_btn) {
          // Do nothing, footage will already be set up this way
        } else if (d.clickedButton() == cancel_btn) {
          // Cancel import
          delete command;
          return;
        }
      }
    }
  }

  if (import_task->HasInvalidFiles()) {
    ProjectImportErrorDialog d(import_task->GetInvalidFiles(), main_window_);
    d.exec();
  }

  undo_stack_.push(command, tr("Imported %1 File(s)").arg(import_task->GetImportedFootage().size()));

  main_window_->SelectFootage(import_task->GetImportedFootage());
}

bool Core::ConfirmImageSequence(const QString& filename) {
  QMessageBox mb(main_window_);

  mb.setIcon(QMessageBox::Question);
  mb.setWindowTitle(tr("Possible image sequence detected"));
  mb.setText(tr("The file '%1' looks like it might be part of an image "
                "sequence. Would you like to import it as such?")
                 .arg(filename));

  mb.addButton(QMessageBox::Yes);
  mb.addButton(QMessageBox::No);

  return (mb.exec() == QMessageBox::Yes);
}

void Core::ProjectWasModified(bool e) { main_window_->setWindowModified(e); }

bool Core::StartHeadlessExport() {
  const QString& startup_project = core_params_.startup_project();

  if (startup_project.isEmpty()) {
    qCritical().noquote() << tr("You must specify a project file to export");
    return false;
  }

  if (!QFileInfo::exists(startup_project)) {
    qCritical().noquote() << tr("Specified project does not exist");
    return false;
  }

  // Start a load task and try running it
  ProjectLoadTask plm(startup_project);
  CLITaskDialog task_dialog(&plm);

  /*
  if (task_dialog.Run()) {
    std::unique_ptr<Project> p = std::unique_ptr<Project>(plm.GetLoadedProject());
    QVector<Item*> items = p->get_items_of_type(Item::kSequence);

    // Check if this project contains sequences
    if (items.isEmpty()) {
      qCritical().noquote() << tr("Project contains no sequences, nothing to export");
      return false;
    }

    Sequence* sequence = nullptr;

    // Check if this project contains multiple sequences
    if (items.size() > 1) {
      qInfo().noquote() << tr("This project has multiple sequences. Which do you wish to export?");
      for (int i=0;i<items.size();i++) {
        std::cout << "[" << i << "] " << items.at(i)->GetLabel().toUtf8().constData();
      }

      QTextStream stream(stdin);
      QString sequence_read;
      int sequence_index = -1;
      QString quit_code = QStringLiteral("q");
      std::string prompt = tr("Enter number (or %1 to cancel): ").arg(quit_code).toUtf8().constData();
      forever {
        std::cout << prompt;

        stream.readLineInto(&sequence_read);

        if (!QString::compare(sequence_read, quit_code, Qt::CaseInsensitive)) {
          return false;
        }

        bool ok;
        sequence_index = sequence_read.toInt(&ok);

        if (ok && sequence_index >= 0 && sequence_index < items.size()) {
          break;
        } else {
          qCritical().noquote() << tr("Invalid sequence number");
        }
      }

      sequence = static_cast<Sequence*>(items.at(sequence_index));
    } else {
      sequence = static_cast<Sequence*>(items.first());
    }

    ExportParams params;
    ExportTask export_task(sequence->viewer_output(), p->color_manager(), params);
    CLITaskDialog export_dialog(&export_task);
    if (export_dialog.Run()) {
      qInfo().noquote() << tr("Export succeeded");
      return true;
    } else {
      qInfo().noquote() << tr("Export failed: %1").arg(export_task.GetError());
      return false;
    }
  } else {
    qCritical().noquote() << tr("Project failed to load: %1").arg(plm.GetError());
    return false;
  }
  */

  return false;
}

void Core::OpenStartupProject() {
  const QString& startup_project = core_params_.startup_project();
  bool startup_project_exists = !startup_project.isEmpty() && QFileInfo::exists(startup_project);

  // Load startup project
  if (!startup_project_exists && !startup_project.isEmpty()) {
    QMessageBox::warning(main_window_, tr("Failed to open startup file"),
                         tr("The project \"%1\" doesn't exist. "
                            "A new project will be started instead.")
                             .arg(startup_project),
                         QMessageBox::Ok);
  }

  if (startup_project_exists) {
    // If a startup project was set and exists, open it now
    OpenProjectInternal(startup_project);
  } else {
    // If no load project is set, create a new one on open
    CreateNewProject();
  }
}

void Core::AddRecoveryProjectFromTask(Task* task) {
  if (AddOpenProjectFromTask(task, false)) {
    auto* load_task = dynamic_cast<ProjectLoadBaseTask*>(task);

    Project* project = load_task->GetLoadedProject();

    // Clearing the filename will force the user to re-save it somewhere else
    project->set_filename(QString());

    // Forcing a UUID regeneration will prevent it from saving auto-recoveries in the same place
    // the original project did
    project->RegenerateUuid();

    // Setting modified will ensure that the program doesn't close and lose the project without
    // prompting the user first
    project->set_modified(true);
  }
}

void Core::StartGUI(bool full_screen) {
  // Set UI style
  StyleManager::Init();

  // Set up shared menus
  MenuShared::CreateInstance();

  // Since we're starting GUI mode, create a PanelFocusManager (auto-deletes with QObject)
  PanelManager::CreateInstance();

  // Initialize audio service
  AudioManager::CreateInstance();

  // Initialize disk service
  DiskManager::CreateInstance();

  // Connect the PanelFocusManager to the application's focus change signal
  connect(qApp, &QApplication::focusChanged, PanelManager::instance(), &PanelManager::FocusChanged);

  // Set KDDockWidgets flags
  auto& config = KDDockWidgets::Config::self();
  auto flags = config.flags();
  flags |= KDDockWidgets::Config::Flag_TabsHaveCloseButton;
  flags |= KDDockWidgets::Config::Flag_HideTitleBarWhenTabsVisible;
  flags |= KDDockWidgets::Config::Flag_AlwaysShowTabs;
  flags |= KDDockWidgets::Config::Flag_AllowReorderTabs;
  config.setFlags(flags);
  KDDockWidgets::Config::setAbsoluteWidgetMinSize(QSize(1, 1));

  // Create main window and open it
  main_window_ = new MainWindow();

  if (full_screen) {
    main_window_->showFullScreen();
  } else {
    main_window_->showMaximized();
  }

#ifdef Q_OS_WINDOWS
  // Workaround for Qt bug where menus don't appear in full screen mode
  // See: https://doc.qt.io/qt-5/windows-issues.html
  QWindowsWindowFunctions::setHasBorderInFullScreen(main_window_->windowHandle(), true);
#endif

  // Start autorecovery timer using the config value as its interval
  SetAutorecoveryInterval(OLIVE_CONFIG("AutorecoveryInterval").toInt());
  connect(&autorecovery_timer_, &QTimer::timeout, this, &Core::SaveAutorecovery);
  autorecovery_timer_.start();

  // Load recently opened projects list
  {
    QFile recent_projects_file(GetRecentProjectsFilePath());
    if (recent_projects_file.open(QFile::ReadOnly | QFile::Text)) {
      QString r = QString::fromUtf8(recent_projects_file.readAll());
      if (!r.isEmpty()) {
        recent_projects_ = r.split('\n');
      }
      recent_projects_file.close();
    }

    emit OpenRecentListChanged();
  }
}

void Core::SaveProjectInternal(const QString& override_filename) {
  // Create save manager
  Task* psm;

  if (open_project_->filename().endsWith(QStringLiteral(".otio"), Qt::CaseInsensitive)) {
#ifdef USE_OTIO
    psm = new SaveOTIOTask(open_project_);
#else
    QMessageBox::critical(main_window_, tr("Missing OpenTimelineIO Libraries"),
                          tr("This build was compiled without OpenTimelineIO and therefore "
                             "cannot open OpenTimelineIO files."));
    return;
#endif
  } else {
    bool use_compression = !open_project_->filename().endsWith(QStringLiteral(".ovexml"), Qt::CaseInsensitive);
    psm = new ProjectSaveTask(open_project_, use_compression);
    dynamic_cast<ProjectSaveTask*>(psm)->SetLayout(main_window_->SaveLayout());

    if (!override_filename.isEmpty()) {
      // Set override filename if provided
      dynamic_cast<ProjectSaveTask*>(psm)->SetOverrideFilename(override_filename);
    }
  }

  // We don't use a TaskDialog here because a model save dialog is annoying, particularly when
  // saving auto-recoveries that the user can't anticipate. Doing this in the main thread will
  // cause a brief (but often unnoticeable) pause in the GUI, which, while not ideal, is not that
  // different from what already happened (modal dialog preventing use of the GUI) and in many ways
  // less annoying (doesn't disrupt any current actions or pull focus from elsewhere).
  //
  // Ideally we could do this in a background thread and show progress in the status bar like
  // Microsoft Word, but that would be far more complex. If it becomes necessary in the future,
  // we will look into an approach like that.
  if (psm->Start()) {
    if (override_filename.isEmpty()) {
      ProjectSaveSucceeded(psm);
    }
  }

  psm->deleteLater();
}

ViewerOutput* Core::GetSequenceToExport() {
  // First try the most recently focused time based window
  auto* time_panel = PanelManager::instance()->MostRecentlyFocused<TimeBasedPanel>();

  // If that fails try defaulting to the first timeline (i.e. if a project has just been loaded).
  if (!time_panel->GetConnectedViewer()) {
    // Safe to assume there will always be one timeline.
    time_panel = PanelManager::instance()->GetPanelsOfType<TimelinePanel>().first();
  }

  if (time_panel && time_panel->GetConnectedViewer()) {
    if (time_panel->GetConnectedViewer()->GetLength() == rational(0)) {
      QMessageBox::critical(main_window_, tr("Error"), tr("This Sequence is empty. There is nothing to export."),
                            QMessageBox::Ok);
    } else {
      return time_panel->GetConnectedViewer();
    }
  } else {
    QMessageBox::critical(
        main_window_, tr("Error"),
        tr("No valid sequence detected.\n\nMake sure a sequence is loaded and it has a connected Viewer node."),
        QMessageBox::Ok);
  }

  return nullptr;
}

QString Core::GetAutoRecoveryIndexFilename() {
  return QDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation))
      .filePath(QStringLiteral("unrecovered"));
}

void Core::SaveUnrecoveredList() {
  QFile autorecovery_index(GetAutoRecoveryIndexFilename());

  if (autorecovered_projects_.isEmpty()) {
    // Recovery list is empty, delete file if exists
    if (autorecovery_index.exists()) {
      autorecovery_index.remove();
    }
  } else if (autorecovery_index.open(QFile::WriteOnly)) {
    // Overwrite recovery list with current list
    QTextStream ts(&autorecovery_index);

    bool first = true;
    foreach (const QUuid& uuid, autorecovered_projects_) {
      if (first) {
        first = false;
      } else {
        ts << QStringLiteral("\n");
      }
      ts << uuid.toString();
    }

    autorecovery_index.close();
  } else {
    qWarning() << "Failed to save unrecovered list";
  }
}

bool Core::RevertProjectInternal(bool by_opening_existing) {
  if (open_project_->filename().isEmpty()) {
    QMessageBox::critical(
        main_window_, tr("Revert"),
        tr("This project has not yet been saved, therefore there is no last saved state to revert to."));
  } else {
    QString msg;

    if (by_opening_existing) {
      msg = tr("The project \"%1\" is already open. By re-opening it, the project will revert to "
               "its last saved state. Any unsaved changes will be lost. Do you wish to continue?")
                .arg(open_project_->filename());
    } else {
      msg = tr("This will revert the project \"%1\" back to its last saved state. "
               "All unsaved changes will be lost. Do you wish to continue?")
                .arg(open_project_->name());
    }

    if (QMessageBox::question(main_window_, tr("Revert"), msg, QMessageBox::Ok | QMessageBox::Cancel) ==
        QMessageBox::Ok) {
      // Copy filename because CloseProject is going to delete `p`
      QString filename = open_project_->filename();

      // Close project without prompting to save it
      CloseProject(false, true);

      // NOTE: `open_project_` will be deleted now, so don't try accessing it

      // Re-open project at the filename
      OpenProjectInternal(filename);

      return true;
    }
  }

  return false;
}

void Core::SaveRecentProjectsList() {
  // Save recently opened projects
  QFile recent_projects_file(GetRecentProjectsFilePath());
  if (recent_projects_file.open(QFile::WriteOnly | QFile::Text)) {
    recent_projects_file.write(recent_projects_.join('\n').toUtf8());
    recent_projects_file.close();
  }
}

void Core::SaveAutorecovery() {
  if (OLIVE_CONFIG("AutorecoveryEnabled").toBool()) {
    if (open_project_ && !open_project_->has_autorecovery_been_saved()) {
      QDir project_autorecovery_dir(
          QDir(FileFunctions::GetAutoRecoveryRoot()).filePath(open_project_->GetUuid().toString()));
      if (FileFunctions::DirectoryIsValid(project_autorecovery_dir)) {
        QString this_autorecovery_path = project_autorecovery_dir.filePath(
            QStringLiteral("%1.ove").arg(QString::number(QDateTime::currentSecsSinceEpoch())));

        SaveProjectInternal(this_autorecovery_path);

        open_project_->set_autorecovery_saved(true);

        // Keep track of projects that where the "newest" save is the recovery project
        if (!autorecovered_projects_.contains(open_project_->GetUuid())) {
          autorecovered_projects_.append(open_project_->GetUuid());
        }

        qDebug() << "Saved auto-recovery to:" << this_autorecovery_path;

        // Write human-readable real name so it's not just a UUID
        {
          QFile realname_file(project_autorecovery_dir.filePath(QStringLiteral("realname.txt")));
          realname_file.open(QFile::WriteOnly);
          realname_file.write(open_project_->pretty_filename().toUtf8());
          realname_file.close();
        }

        int64_t max_recoveries_per_file = OLIVE_CONFIG("AutorecoveryMaximum").toLongLong();

        // Since we write an extra file, increment total allowed files by 1
        max_recoveries_per_file++;

        // Delete old entries
        QStringList recovery_files = project_autorecovery_dir.entryList(QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
        while (recovery_files.size() > max_recoveries_per_file) {
          bool deleted = false;
          for (int i = 0; i < recovery_files.size(); i++) {
            const QString& f = recovery_files.at(i);

            if (f.endsWith(QStringLiteral(".ove"), Qt::CaseInsensitive)) {
              QString delete_full_path = project_autorecovery_dir.filePath(f);
              qDebug() << "Deleted old recovery:" << delete_full_path;
              QFile::remove(delete_full_path);
              recovery_files.removeAt(i);
              deleted = true;
              break;
            }
          }

          if (!deleted) {
            // For some reason none of the files were deletable. Break so we don't end up in
            // an infinite loop.
            break;
          }
        }
      } else {
        QMessageBox::critical(main_window_, tr("Auto-Recovery Error"),
                              tr("Failed to save auto-recovery to \"%1\". "
                                 "Olive may not have permission to this directory.")
                                  .arg(project_autorecovery_dir.absolutePath()));
      }
    }

    // Save index
    SaveUnrecoveredList();
  }
}

void Core::ProjectSaveSucceeded(Task* task) {
  Project* p = dynamic_cast<ProjectSaveTask*>(task)->GetProject();

  PushRecentlyOpenedProject(p->filename());

  p->set_modified(false);

  autorecovered_projects_.removeOne(p->GetUuid());
  SaveUnrecoveredList();

  ShowStatusBarMessage(tr("Saved to \"%1\" successfully").arg(p->filename()));
}

Project* Core::GetActiveProject() const { return open_project_; }

Folder* Core::GetSelectedFolderInActiveProject() {
  auto* active_project_panel = PanelManager::instance()->MostRecentlyFocused<ProjectPanel>();

  if (active_project_panel) {
    return active_project_panel->GetSelectedFolder();
  } else {
    return nullptr;
  }
}

Timecode::Display Core::GetTimecodeDisplay() {
  return static_cast<Timecode::Display>(OLIVE_CONFIG("TimecodeDisplay").toInt());
}

void Core::SetTimecodeDisplay(Timecode::Display d) {
  OLIVE_CONFIG("TimecodeDisplay") = d;

  emit TimecodeDisplayChanged(d);
}

void Core::SetAutorecoveryInterval(int minutes) {
  // Convert minutes to milliseconds
  autorecovery_timer_.setInterval(minutes * 60000);
}

void Core::CopyStringToClipboard(const QString& s) { QGuiApplication::clipboard()->setText(s); }

QString Core::PasteStringFromClipboard() { return QGuiApplication::clipboard()->text(); }

QString Core::GetProjectFilter(bool include_any_filter) {
  static const QVector<QPair<QString, QString> > FILTERS = {
      // Standard compressed Olive project
      {tr("Olive Project"), QStringLiteral("ove")},

      // Uncompressed XML Olive project
      {tr("Olive Project (Uncompressed XML)"), QStringLiteral("ovexml")},

  // OpenTimelineIO project, if available
#ifdef USE_OTIO
      {tr("OpenTimelineIO"), QStringLiteral("otio")}
#endif
  };

  QStringList filters;
  filters.reserve(FILTERS.size() + 1);

  if (include_any_filter) {
    QStringList combined;
    for (const auto& it : FILTERS) {
      combined.append(QStringLiteral("*.%1").arg(it.second));
    }
    filters.append(QStringLiteral("%1 (%2)").arg(tr("All Supported Projects"), combined.join(' ')));
  }

  for (const auto& it : FILTERS) {
    filters.append(QStringLiteral("%1 (*.%2)").arg(it.first, it.second));
  }

  return filters.join(QStringLiteral(";;"));
}

QString Core::GetRecentProjectsFilePath() {
  return QDir(FileFunctions::GetConfigurationLocation()).filePath(QStringLiteral("recent"));
}

void Core::SetStartupLocale() {
  // Set language
  if (!core_params_.startup_language().isEmpty()) {
    if (translator_->load(core_params_.startup_language()) && QApplication::installTranslator(translator_)) {
      return;
    } else {
      qWarning() << "Failed to load translation file. Falling back to defaults.";
    }
  }

  QString use_locale = OLIVE_CONFIG("Language").toString();

  if (use_locale.isEmpty()) {
    // No configured locale, auto-detect the system's locale
    use_locale = QLocale::system().name();
  }

  if (!SetLanguage(use_locale)) {
    qWarning() << "Trying to use locale" << use_locale << "but couldn't find a translation for it";
  }
}

bool Core::SaveProject() {
  if (open_project_->filename().isEmpty()) {
    return SaveProjectAs();
  } else {
    SaveProjectInternal();

    return true;
  }
}

void Core::ShowStatusBarMessage(const QString& s, int timeout) { main_window_->statusBar()->showMessage(s, timeout); }

void Core::ClearStatusBarMessage() { main_window_->statusBar()->clearMessage(); }

void Core::OpenRecoveryProject(const QString& filename) { OpenProjectInternal(filename, true); }

void Core::OpenNodeInViewer(ViewerOutput* viewer) { main_window_->OpenNodeInViewer(viewer); }

void Core::OpenExportDialogForViewer(ViewerOutput* viewer, bool start_still_image) {
  auto* ed = new ExportDialog(viewer, start_still_image, main_window_);
  connect(ed, &ExportDialog::finished, ed, &ExportDialog::deleteLater);
  ed->open();
  connect(ed, &ExportDialog::RequestImportFile, this, &Core::ImportSingleFile);
}

void Core::CheckForAutoRecoveries() {
  QFile autorecovery_index(GetAutoRecoveryIndexFilename());
  if (autorecovery_index.exists()) {
    // Uh-oh, we have auto-recoveries to prompt
    if (autorecovery_index.open(QFile::ReadOnly)) {
      QStringList recovery_filenames = QString::fromUtf8(autorecovery_index.readAll()).split('\n');

      AutoRecoveryDialog ard(tr("The following projects had unsaved changes when Olive "
                                "forcefully quit. Would you like to load them?"),
                             recovery_filenames, true, main_window_);
      ard.exec();

      autorecovery_index.close();

      // Delete recovery index since we don't need it anymore
      QFile::remove(GetAutoRecoveryIndexFilename());
    } else {
      QMessageBox::critical(main_window_, tr("Auto-Recovery Error"),
                            tr("Found auto-recoveries but failed to load the auto-recovery index. "
                               "Auto-recover projects will have to be opened manually.\n\n"
                               "Your recoverable projects are still available at: %1")
                                .arg(FileFunctions::GetAutoRecoveryRoot()));
    }
  }
}

void Core::BrowseAutoRecoveries() {
  // List all auto-recovery entries
  AutoRecoveryDialog ard(tr("The following project versions have been auto-saved:"),
                         QDir(FileFunctions::GetAutoRecoveryRoot()).entryList(QDir::Dirs | QDir::NoDotAndDotDot), false,
                         main_window_);
  ard.exec();
}

void Core::RequestPixelSamplingInViewers(bool e) {
  if (e) {
    if (pixel_sampling_users_ == 0) {
      // Signal to start pixel sampling
      emit ColorPickerEnabled(true);
    }

    pixel_sampling_users_++;
  } else {
    pixel_sampling_users_--;

    if (pixel_sampling_users_ == 0) {
      // Signal to end pixel sampling
      emit ColorPickerEnabled(false);
    }
  }
}

void Core::WarnCacheFull() {
  if (!shown_cache_full_warning_ && main_window_) {
    shown_cache_full_warning_ = true;

    QMessageBox::warning(main_window_, tr("Disk Cache Full"),
                         tr("The disk cache is currently full and Olive is having to delete old "
                            "frames to keep it within the limits set in the Disk preferences. This "
                            "will result in SIGNIFICANTLY reduced cache performance.\n\n"
                            "To remedy this, please do one of the following:\n\n"
                            "1. Manually clear the disk cache in Disk preferences.\n"
                            "2. Increase the maximum disk cache size in Disk preferences.\n"
                            "3. Reduce usage of the disk cache (e.g. disable auto-cache or only cache specific "
                            "sections of your sequence)."));
  }
}

bool Core::SaveProjectAs() {
  QFileDialog fd(main_window_, tr("Save Project As"));

  fd.setAcceptMode(QFileDialog::AcceptSave);
  fd.setNameFilter(GetProjectFilter(false));

  if (fd.exec() == QDialog::Accepted) {
    QString fn = fd.selectedFiles().first();

    // Somewhat hacky method of extracting the extension from the name filter
    const QString& name_filter = fd.selectedNameFilter();
    int ext_index = name_filter.indexOf(QStringLiteral("(*.")) + 3;
    QString extension = name_filter.mid(ext_index, name_filter.size() - ext_index - 1);

    fn = FileFunctions::EnsureFilenameExtension(fn, extension);

    open_project_->set_filename(fn);

    SaveProjectInternal();

    return true;
  }

  return false;
}

void Core::RevertProject() { RevertProjectInternal(false); }

void Core::PushRecentlyOpenedProject(const QString& s) {
  if (s.isEmpty()) {
    return;
  }

  int existing_index = recent_projects_.indexOf(s);

  if (existing_index >= 0) {
    recent_projects_.move(existing_index, 0);
  } else {
    recent_projects_.prepend(s);

    const int kMaximumRecentProjects = 10;
    while (recent_projects_.size() > kMaximumRecentProjects) {
      recent_projects_.removeLast();
    }
  }

  SaveRecentProjectsList();

  emit OpenRecentListChanged();
}

void Core::OpenProjectInternal(const QString& filename, bool recovery_project) {
  if (open_project_) {
    // Comparing QFileInfos will handle case insensitivity and both slash directions on platforms
    // where this is necessary (not naming any names *cough* Windows)
    if (QFileInfo(open_project_->filename()) == QFileInfo(filename)) {
      // This project is already open
      bool reverted = RevertProjectInternal(true);

      if (!reverted) {
        // Calling this will focus attention to the project that the user just tried to re-open
        AddOpenProject(open_project_);
      }

      // Don't do anything else
      return;
    }
  }

  Task* load_task;

  if (filename.endsWith(QStringLiteral(".otio"), Qt::CaseInsensitive)) {
    // Load OpenTimelineIO project
#ifdef USE_OTIO
    load_task = new LoadOTIOTask(filename);
#else
    QMessageBox::critical(main_window_, tr("Missing OpenTimelineIO Libraries"),
                          tr("This build was compiled without OpenTimelineIO and therefore "
                             "cannot open OpenTimelineIO files."));
    return;
#endif
  } else {
    // Fallback to regular OVE project
    load_task = new ProjectLoadTask(filename);
  }

  auto* task_dialog = new TaskDialog(load_task, tr("Load Project"), main_window());

  if (recovery_project) {
    connect(task_dialog, &TaskDialog::TaskSucceeded, this, &Core::AddRecoveryProjectFromTask);
  } else {
    connect(task_dialog, &TaskDialog::TaskSucceeded, this, &Core::AddOpenProjectFromTaskAndAddToRecents);
  }

  task_dialog->open();
}

void Core::ImportSingleFile(const QString& f) {
  if (Project* p = GetActiveProject()) {
    ImportFiles({f}, p->root());
  }
}

int Core::CountFilesInFileList(const QFileInfoList& filenames) {
  int file_count = 0;

  foreach (const QFileInfo& f, filenames) {
    // For some reason QDir::NoDotAndDotDot	doesn't work with entryInfoList, so we have to check manually
    if (f.fileName() == "." || f.fileName() == "..") {
      continue;
    } else if (f.isDir()) {
      QFileInfoList info_list = QDir(f.absoluteFilePath()).entryInfoList();

      file_count += CountFilesInFileList(info_list);
    } else {
      file_count++;
    }
  }

  return file_count;
}

bool Core::LabelNodes(const QVector<Node*>& nodes, MultiUndoCommand* parent) {
  if (nodes.isEmpty()) {
    return false;
  }

  bool ok;

  QString start_label = nodes.first()->GetLabel();

  for (int i = 1; i < nodes.size(); i++) {
    if (nodes.at(i)->GetLabel() != start_label) {
      // Not all the nodes share the same name, so we'll start with a blank one
      start_label.clear();
      break;
    }
  }

  QString s =
      QInputDialog::getText(main_window_, tr("Label Node"), tr("Set node label"), QLineEdit::Normal, start_label, &ok);

  if (ok) {
    auto* rename_command = new NodeRenameCommand();

    foreach (Node* n, nodes) {
      rename_command->AddNode(n, s);
    }

    if (parent) {
      parent->add_child(rename_command);
    } else {
      undo_stack_.push(rename_command, tr("Renamed %1 Node(s)").arg(nodes.size()));
    }

    return true;
  }

  return false;
}

Sequence* Core::CreateNewSequenceForProject(const QString& format, Project* project) {
  auto* new_sequence = new Sequence();

  // Get default name for this sequence (in the format "Sequence N", the first that doesn't exist)
  int sequence_number = 1;
  QString sequence_name;
  do {
    sequence_name = format.arg(sequence_number);
    sequence_number++;
  } while (project->root()->ChildExistsWithName(sequence_name));
  new_sequence->SetLabel(sequence_name);

  return new_sequence;
}

void Core::OpenProjectFromRecentList(int index) {
  const QString& open_fn = recent_projects_.at(index);

  if (QFileInfo::exists(open_fn)) {
    OpenProjectInternal(open_fn);
  } else if (QMessageBox::information(
                 main_window(), tr("Cannot open recent project"),
                 tr("The project \"%1\" doesn't exist. Would you like to remove this file from the recent list?")
                     .arg(open_fn),
                 QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
    recent_projects_.removeAt(index);

    SaveRecentProjectsList();

    emit OpenRecentListChanged();
  }
}

bool Core::CloseProject(bool auto_open_new, bool ignore_modified) {
  if (open_project_) {
    if (open_project_->is_modified() && !ignore_modified) {
      QMessageBox mb(main_window_);

      mb.setWindowModality(Qt::WindowModal);
      mb.setIcon(QMessageBox::Question);
      mb.setWindowTitle(tr("Unsaved Changes"));
      mb.setText(tr("The project '%1' has unsaved changes. Would you like to save them?").arg(open_project_->name()));

      QPushButton* yes_btn = mb.addButton(tr("Save"), QMessageBox::YesRole);

      mb.addButton(tr("Don't Save"), QMessageBox::NoRole);

      QPushButton* cancel_btn = mb.addButton(QMessageBox::Cancel);

      mb.exec();

      if (mb.clickedButton() == cancel_btn) {
        // Stop closing projects if the user clicked cancel
        return false;
      }

      if (mb.clickedButton() == yes_btn && !SaveProject()) {
        // The save failed, stop closing projects
        return false;
      }
    }

    // For safety, the undo stack is cleared so no commands try to affect a freed project
    undo_stack_.clear();

    Project* tmp = open_project_;
    SetActiveProject(nullptr);
    delete tmp;
  }

  // Ensure a project is always active
  if (auto_open_new) {
    CreateNewProject();
  }

  return true;
}

void Core::CacheActiveSequence(bool in_out_only) {
  auto* p = PanelManager::instance()->MostRecentlyFocused<TimeBasedPanel>();

  if (p && p->GetConnectedViewer()) {
    // Hacky but works for now

    // Find Viewer attached to this TimeBasedPanel
    QList<ViewerPanel*> all_viewers = PanelManager::instance()->GetPanelsOfType<ViewerPanel>();

    ViewerPanel* found_panel = nullptr;

    foreach (ViewerPanel* viewer, all_viewers) {
      if (viewer->GetConnectedViewer() == p->GetConnectedViewer()) {
        found_panel = viewer;
        break;
      }
    }

    if (found_panel) {
      if (in_out_only) {
        found_panel->CacheSequenceInOut();
      } else {
        found_panel->CacheEntireSequence();
      }
    } else {
      QMessageBox::critical(main_window_, tr("Failed to cache sequence"),
                            tr("No active viewer found with this sequence."), QMessageBox::Ok);
    }
  }
}

QString StripWindowsDriveLetter(QString s) {
  // HACK: On Windows, absolute paths are saved with a drive letter (e.g. "C:\video.mp4"). Below,
  //       we use Qt's relative path system to resolve when an entire project may be in a different
  //       folder, but the files are all in the same place relatively to the project. Unfortunately,
  //       Qt chooses not to understand paths from Windows on non-Windows platforms, which causes
  //       this to break when a project is moving from Windows to non-Windows. To resolve that, if
  //       we're on a non-Windows platform and we detect a Windows path (i.e. a path with a drive
  //       letter at the start), we strip it off. We also convert any back-slashes to forward-slashes
  //       because on Windows they are interchangeable and on non-Windows they are not.
#ifndef Q_OS_WINDOWS
  if (s.size() >= 2) {
    if (s.at(0).isLetter() && s.at(1) == ':') {
      s = s.mid(2);
      s.replace('\\', '/');
    }
  }
#endif

  return s;
}

bool Core::ValidateFootageInLoadedProject(Project* project, const QString& project_saved_url) {
  QVector<Footage*> footage_we_couldnt_validate;

  for (Node* n : project->nodes()) {
    if (auto* footage = dynamic_cast<Footage*>(n)) {
      QString footage_fn = StripWindowsDriveLetter(footage->filename());
      QString project_fn = StripWindowsDriveLetter(project_saved_url);

      if (!QFileInfo::exists(footage_fn) && !project_saved_url.isEmpty()) {
        // If the footage doesn't exist, it might have moved with the project
        const QString& project_current_url = project->filename();

        if (project_current_url != project_fn) {
          // Project has definitely moved, try to resolve relative paths
          QDir saved_dir(QFileInfo(project_fn).dir());
          QDir true_dir(QFileInfo(project_current_url).dir());

          QString relative_filename = saved_dir.relativeFilePath(footage_fn);
          QString transformed_abs_filename = true_dir.filePath(relative_filename);

          if (QFileInfo::exists(transformed_abs_filename)) {
            // Use this file instead
            qInfo() << "Resolved" << footage_fn << "relatively to" << transformed_abs_filename;
            footage->set_filename(transformed_abs_filename);
          }
        }
      }

      if (QFileInfo::exists(footage->filename())) {
        // Assume valid
        footage->SetValid();
      } else {
        footage_we_couldnt_validate.append(footage);
      }
    }
  }

  if (!footage_we_couldnt_validate.isEmpty()) {
    FootageRelinkDialog frd(footage_we_couldnt_validate, main_window_);
    if (frd.exec() == QDialog::Rejected) {
      return false;
    }
  }

  return true;
}

bool Core::SetLanguage(const QString& locale) {
  QApplication::removeTranslator(translator_);

  QString resource_path = QStringLiteral(":/ts/%1").arg(locale);
  if (translator_->load(resource_path) && QApplication::installTranslator(translator_)) {
    return true;
  }

  return false;
}

void Core::OpenProject() {
  QString file = QFileDialog::getOpenFileName(main_window_, tr("Open Project"), QString(), GetProjectFilter(true));

  if (!file.isEmpty()) {
    OpenProjectInternal(file);
  }
}

Core::CoreParams::CoreParams() : mode_(kRunNormal), run_fullscreen_(false), crash_(false) {}

}  // namespace olive
