#include "commandlineparser.h"

#include <QCoreApplication>
#include <QDebug>

CommandLineParser::~CommandLineParser() {
  foreach (const KnownOption& o, options_) {
    delete o.option;
  }

  foreach (const KnownPositionalArgument& a, positional_args_) {
    delete a.option;
  }
}

const CommandLineParser::Option* CommandLineParser::AddOption(const QStringList& strings, const QString& description,
                                                              bool takes_arg, const QString& arg_placeholder,
                                                              bool hidden) {
  auto* o = new Option();

  options_.append({strings, description, o, takes_arg, arg_placeholder, hidden});

  return o;
}

const CommandLineParser::PositionalArgument* CommandLineParser::AddPositionalArgument(const QString& name,
                                                                                      const QString& description,
                                                                                      bool required) {
  auto* a = new PositionalArgument();

  positional_args_.append({name, description, a, required});

  return a;
}

void CommandLineParser::Process(const QVector<QString>& argv) {
  int positional_index = 0;

  for (int i = 1; i < argv.size(); i++) {
    if (argv[i][0] == '-') {
      // Must be an option

      // Skip past first dashes
      QString arg_basename = argv[i].mid(1);

      bool matched_known = false;

      for (auto& o : options_) {
        foreach (const QString& s, o.args) {
          if (!s.compare(arg_basename, Qt::CaseInsensitive)) {
            // Flag discovered!
            o.option->Set();

            if (o.takes_arg && i + 1 < argv.size()) {
              o.option->SetSetting(argv[i + 1]);
              i++;
            }

            matched_known = true;
            goto found_flag;
          }
        }
      }

    found_flag:
      if (!matched_known) {
        qWarning() << "Unknown parameter:" << argv[i];
      }

    } else {
      // Must be a positional flag
      if (positional_index < positional_args_.size()) {
        positional_args_[positional_index].option->SetSetting(argv[i]);
        positional_index++;
      } else {
        qWarning() << "Unknown parameter:" << argv[i];
      }
    }
  }
}

void CommandLineParser::PrintHelp(const char* filename) {
  printf("%s %s\n", QCoreApplication::applicationName().toUtf8().constData(),
         QCoreApplication::applicationVersion().toUtf8().constData());

  printf("Copyright (C) 2018-2022 Olive Team\n");

  QString positional_args;
  for (int i = 0; i < positional_args_.size(); i++) {
    if (i > 0) {
      positional_args.append(' ');
    }

    positional_args.append('[');
    positional_args.append(positional_args_.at(i).name);
    positional_args.append(']');
  }

  const char* basename;
#ifdef Q_OS_WINDOWS
  basename = strrchr(filename, '\\');
  if (!basename) {
    basename = strrchr(filename, '/');
  }
#else
  basename = strrchr(filename, '/');
#endif

  if (basename) {
    // Slash found, increment pointer to avoid showing the slash itself
    basename++;
  } else {
    // If no slashes are found, assume string is already a basename
    basename = filename;
  }

  printf("Usage: %s [options] %s\n\n", basename, positional_args.toUtf8().constData());
  foreach (const KnownOption& o, options_) {
    if (o.hidden) {
      continue;
    }

    QString all_args;

    for (int i = 0; i < o.args.size(); i++) {
      if (i > 0) {
        all_args.append(QStringLiteral(", "));
      }

      const QString& this_arg = o.args.at(i);

      all_args.append('-');
      all_args.append(this_arg);
    }

    if (o.arg_placeholder.isEmpty()) {
      printf("    %s\n", all_args.toUtf8().constData());
    } else {
      printf("    %s <%s>\n", all_args.toUtf8().constData(), o.arg_placeholder.toUtf8().constData());
    }

    printf("        %s\n\n", o.description.toUtf8().constData());
  }

  printf("\n");
}
