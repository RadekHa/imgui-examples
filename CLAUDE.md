# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project overview

Small C++20 ImGui/SDL2 demo app with a webcam viewer. Project name is `BasicGuiProjectSetup` (CMake `project()`); the executable target is `App`. Dependencies (SDL2, Dear ImGui `docking`, Catch2, spdlog, Tracy, stb_image, OpenCV) are pulled in via `FetchContent` from `vendor/CMakeLists.txt`. OpenCV is configured to build only `core,imgproc,videoio` (static, no tests/examples/Python).

## Build

CMake presets live in `CMakePresets.json`. Two flows:

- Configure + build (Ninja, multi-config unsupported in this layout):
  ```bash
  cmake --preset debug
  cmake --build --preset debug
  ```
- Distribution (configure → build `App` → package):
  ```bash
  cmake --workflow --preset dist
  ```

`debug` preset enables `TRACY_ENABLE`. `release` preset's `buildPresets` only targets `App`. `xcode-*` presets are Darwin-only. The project requires `cmake >= 3.22` and emits `compile_commands.json` (`StandardProjectSettings.cmake`).

AddressSanitizer + `-O0 -g` are added globally for non-Release, non-Windows builds (`cmake/StaticAnalyzers.cmake`). `clang-tidy` is also wired in for non-Release. MSVC warning level is `/W4` plus a long flag list (`cmake/CompilerWarnings.cmake`); `WARNINGS_AS_ERRORS` is exposed as a cache option.

### Common CMake options

- `WARNINGS_AS_ERRORS` — treat warnings as errors.
- `DEACTIVATE_LOGGING` — adds `APP_DEACTIVATE_LOGGING`.
- `DEBUG` — adds `DEBUG` and `APP_PROFILE` defines.

## Tests

`enable_testing()` is on at the top level. The `all` test preset uses the `debug` configure preset:

```bash
ctest --preset all
```

Two test targets:

- `CoreTest` (executable) — Catch2, `src/core/Tests/{EventBusTest,Resources.spec}.cpp`, discovers tests via `catch_discover_tests`. Linked against `Core` and the shared `TestRunner` OBJECT library.
- `TestRunner` (OBJECT) — Catch2 test fixtures shared across executables. Located in `src/tests/`.

Run a single Catch2 test by tag or name with the standard Catch2 CLI: `./build/debug/src/core/Tests/CoreTest "Core::EventBus"` (or use `ctest -R Core::EventBus -V`).

## Module layout (src/)

Each module is its own CMake target under `src/<module>/` with a `<Module>/CMakeLists.txt`. Public headers are exposed via `target_include_directories(... PUBLIC <dir>)`; `.cpp` files are kept under a `src/` subfolder and added with `PRIVATE` include dirs to discourage leaking them as public includes. Where you see `Core/X.h` referenced as a public include, the public include path is `src/core/` and the file lives in `src/core/Core/`.

| Module | Target | Notes |
| --- | --- | --- |
| `app` | executable `App` | `src/app/App/Main.cpp`. Links `Core`. Pulls in `app.rc` / `.manifest` / `.desktop.in` / `Info.plist` from `src/app/Manifests/` and the icon/font assets via `src/app/cmake/AppAssets.cmake`. |
| `core` | static `Core` | The `Application` class. Depends on `Ui`, `Types`, `Sdl`, `Camera`, `TraceLog`, `Events`. |
| `sdl` | static `Sdl` | SDL/ImGui glue. Window, renderer, event translator, ImGui pass, DPI handler, camera texture. Depends on `SDL2::SDL2`, `Events`, `Types`, `TraceLog`, `Image`, `Camera`, `imgui`, `Settings`. |
| `ui` | static `Ui` | ImGui state machine. `IAppUi` (factory `createAppUi`) → `AppUi`; `IUiState` → `StateStart`/`StateLogin`/`StateNull`. Depends on `Types`, `imgui`, `TraceLog`. |
| `events` | INTERFACE `Events` | Header-only. Event bus implementation and event type definitions (`Events/Bus.h`, `Events/BusImpl.h`, `Events/Events.h`). Defines `EventBus` and events: `EventQuit`, `EventClose`, `EventMinimized`, `EventShown`, `EventRestored`, `EventDisplayChanged`. |
| `types` | INTERFACE `Types` | Header-only. `DataModel.h`, `ImageInfo.h`. |
| `image` | static `Image` | Image loading via stb_image. Depends on `TraceLog`, `stb::image`. |
| `camera` | static `Camera` | OpenCV-backed camera capture (`ICamera`, factory `createCamera(0)`). Depends on `TraceLog`, `opencv::{core,videoio,imgproc}`. |
| `settings` | static `Settings` | `Project.cpp.in` is `configure_file`'d at configure time to inject `CMAKE_PROJECT_NAME` and `PROJECT_COMPANY_NAMESPACE` into `App::APP_NAME` / `App::COMPANY_NAMESPACE`. |
| `traceLog` | static `TraceLog` | `spdlog` wrapper + `Tracy.hpp` (`ZoneScoped` macros used across the app). Depends on `Tracy::TracyClient`, `spdlog`. |
| `tests` | OBJECT `TestRunner` | Shared Catch2 test objects. |

The dependency order at the top of `src/CMakeLists.txt` is: `events` → `traceLog` → `types` → `ui` → `sdl` → `settings` → `image` → `camera` → `core` → `app` → `tests`. New modules need to be added here.

## Runtime architecture

`main()` in `src/app/App/Main.cpp` builds an `SdlPathService` + `SdlContext`, then constructs `App::Application` and calls `run()`.

`App::Application` (in `src/core/Core/`) owns the SDL window/renderer, the ImGui pass, the camera, the data model, and the UI. Per frame it:

1. Reads a camera frame into `SdlCameraTexture` (via `Camera::ICamera`).
2. Polls SDL events, hands them to `SdlEventTranslator`, which `publish()`es typed events onto the `App::EventBus`.
3. Begins an ImGui frame, copies the current camera `ImageInfo` into `DataModel::camera`, then calls `Ui::IAppUi::update` which drives the `IUiState` state machine.
4. Ends ImGui + renderer frame.
5. `m_bus.dispatch()` — all queued events fire.

The event bus (`App::EventBusImpl<Events>`) is a queue-based variant dispatcher. Subscribers register lambdas or member functions and get back an RAII `Subscription` that auto-unsubscribes on destruction. `EventBus::Subscription` is move-only. `EventBus` is defined in `Events/Bus.h`; the event types live in `Events/Events.h` (`EventQuit`, `EventClose`, `EventMinimized`, `EventShown`, `EventRestored`, `EventDisplayChanged`). The `Events` module provides the bus implementation; `Events/Bus.h` re-exports the bus under the `App::` namespace.

`DataModel` (in `Types/DataModel.h`) is the per-frame state object shared between `Core` and `Ui`. New fields that the UI needs to read go there.

## Conventions

- C++20; `cxx_std_20` is set per-target. `clang-tidy` runs on non-Release builds.
- Headers use `#pragma once`.
- Doxygen-style `/** ... */` comments are used throughout the existing code; match the surrounding style when adding new public APIs.
- Public include paths mirror the module directory: `Types/DataModel.h` is included as `Types/DataModel.h`; `Core/Application.hpp` as `Core/Application.hpp`.
- Factory functions returning `std::unique_ptr<T>` / raw `IAppUi*` are used at module boundaries (`createSdlPathService`, `createAppUi`, `createCamera`).
- Settings: the `App::APP_NAME` / `App::COMPANY_NAMESPACE` constants come from `Project.cpp` (generated by `configure_file` from `Project.cpp.in`).
