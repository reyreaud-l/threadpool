find_package(Doxygen REQUIRED)

set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
doxygen_add_docs(doc ${CMAKE_SOURCE_DIR}/include COMMENT "Generating API documentation with Doxygen")
