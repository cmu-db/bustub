#include <argparse/argparse.hpp>
#include <doctest.hpp>

using doctest::test_suite;

TEST_CASE("Issues with implicit values #37" * test_suite("implicit_values")) {
  argparse::ArgumentParser m_bfm("test");
  m_bfm.add_argument("-l", "--load").help("load a VMM into the kernel");

  m_bfm.add_argument("-u", "--unload")
      .default_value(false)
      .implicit_value(true)
      .help("unload a previously loaded VMM");

  m_bfm.add_argument("-x", "--start")
      .default_value(false)
      .implicit_value(true)
      .help("start a previously loaded VMM");

  m_bfm.add_argument("-s", "--stop")
      .default_value(false)
      .implicit_value(true)
      .help("stop a previously started VMM");

  m_bfm.add_argument("-d", "--dump")
      .default_value(false)
      .implicit_value(true)
      .help("output the contents of the VMM's debug buffer");

  m_bfm.add_argument("-m", "--mem")
      .default_value(100)
      .required()
      .scan<'u', unsigned long long>()
      .help("memory in MB to give the VMM when loading");
  m_bfm.parse_args({"test", "-l", "blah", "-d", "-u"});

  REQUIRE(m_bfm.get("--load") == "blah");
  REQUIRE(m_bfm.get("-l") == "blah");
  REQUIRE(m_bfm.get<bool>("-u") == true);
  REQUIRE(m_bfm.get<bool>("-d") == true);
  REQUIRE(m_bfm.get<bool>("-s") == false);
  REQUIRE(m_bfm.get<bool>("--unload") == true);
}
