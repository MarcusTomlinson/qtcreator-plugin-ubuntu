project(%ProjectName:l%)
cmake_minimum_required(VERSION 2.8.10)

# This command figures out the target architecture
execute_process(
  COMMAND dpkg-architecture -qDEB_HOST_ARCH
  OUTPUT_VARIABLE CLICK_ARCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Do not remove this, its required to ensure the correct node binaries are included in the project
execute_process(COMMAND unity-js-scopes-tool install ${CMAKE_SOURCE_DIR}/src/node_modules unity-js-scopes)
add_custom_target(rebuild_modules ALL COMMAND unity-js-scopes-tool rebuild ${CMAKE_SOURCE_DIR}/src/node_modules ${CLICK_ARCH})

# Do not remove this, its required for the correct functionality of the Ubuntu-SDK
set(UBUNTU_MANIFEST_PATH "manifest.json.in" CACHE INTERNAL "Tells QtCreator location and name of the manifest file")
set(UBUNTU_PROJECT_TYPE "Scope" CACHE INTERNAL "Tells QtCreator this is a Scope project")

# Important project paths
set(CMAKE_INSTALL_PREFIX /)
set(SCOPE_INSTALL_DIR "/%ClickHookName:l%")
set(GETTEXT_PACKAGE "%ProjectName:l%")
set(PACKAGE_NAME "%ProjectName:l%.%ClickDomain:l%")
set(SCOPE_NAME "${PACKAGE_NAME}_%ClickHookName:l%")

# Install the manifest and apparmor files
configure_file(manifest.json.in ${CMAKE_CURRENT_BINARY_DIR}/manifest.json)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/manifest.json DESTINATION "/")
install(FILES "%ClickHookName:l%.apparmor" DESTINATION "/")

# Make these files show up in QtCreator
file(GLOB_RECURSE
  _PO_JS_FILES
  "po/*.po"
  "*.js"
)
add_custom_target(hidden_files
  ALL
  SOURCES
    manifest.json.in
    %ClickHookName:l%.apparmor
    src/data/%ClickHookName:l%.ini.in
    src/data/%ClickHookName:l%-settings.ini.in
    po/POTFILES.in.in
    po/${GETTEXT_PACKAGE}.pot
    ${_PO_JS_FILES}
)

# Search for our dependencies
find_package(Intltool)

# Add our main directories
add_subdirectory(po)
add_subdirectory(src)
