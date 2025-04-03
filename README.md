#### 1. 小型个人项目

- **构建**：CMake。
- **编辑**：VS Code + Clang-Format。
- **调试**：GDB/LLDB + ASan。
- **测试**：Google Test。

#### 2. 中型团队项目

- **构建**：CMake。
- **静态分析**：Clang-Tidy + Cppcheck。
- **调试**：Sanitizers (ASan, UBSan) + Valgrind。
- **测试**：Google Test。
- **格式**：Clang-Format（集成到 CI）。

#### 3. 大型商业项目

- **构建**：CMake 或 Bazel。
- **静态分析**：Clang-Tidy + Coverity。
- **调试**：Sanitizers (ASan, TSan, UBSan) + GDB/LLDB。
- **崩溃捕获**：Crashpad。
- **测试**：Google Test + CI 集成。
- **性能**：Perf 或 VTune。
- **格式**：Clang-Format。

#### 4. 多线程或高并发项目

- 额外工具：**TSan** + **Helgrind (Valgrind)**。

---

### 五、为什么这些工具是“正常开发”必备？

1. **效率**：CMake 和 IDE 加快开发速度。
2. **质量**：Clang-Tidy 和 Sanitizers 减少 Bug。
3. **调试**：GDB/LLDB 和 Valgrind 定位问题。
4. **稳定性**：Crashpad 监控生产环境。
5. **可维护性**：Clang-Format 和测试框架保持代码健康。

---

### 实际开发中的建议

- **初学者**：从 CMake、ASan 和 GDB 开始，逐步加入 Clang-Format 和 Google Test。
- **团队开发**：强制使用 Clang-Format 和 Sanitizers，集成到 CI（如 GitHub Actions）。
- **生产部署**：加上 Crashpad 和 Perf，确保上线后可监控。