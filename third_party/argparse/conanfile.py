from conans import ConanFile

class ArgparseConan(ConanFile):
    name = "argparse"
    version = "2.9"
    exports_sources = "include/argparse.hpp"
    no_copy_source = True

    def package(self):
        self.copy("argparse.hpp")
