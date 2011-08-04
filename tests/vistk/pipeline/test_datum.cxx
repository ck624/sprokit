/*ckwg +5
 * Copyright 2011 by Kitware, Inc. All Rights Reserved. Please refer to
 * KITWARE_LICENSE.TXT for licensing information, or contact General Counsel,
 * Kitware, Inc., 28 Corporate Drive, Clifton Park, NY 12065.
 */

#include <vistk/pipeline/datum.h>

#include <exception>
#include <iostream>

static void run_test(std::string const& test_name);

int
main(int argc, char* argv[])
{
  if (argc != 2)
  {
    std::cerr << "Error: Expected one argument" << std::endl;

    return 1;
  }

  std::string const test_name = argv[1];

  try
  {
    run_test(test_name);
  }
  catch (std::exception& e)
  {
    std::cerr << "Error: Unexpected exception: " << e.what() << std::endl;

    return 1;
  }

  return 0;
}

static void test_empty();
static void test_complete();
static void test_error();

void
run_test(std::string const& test_name)
{
  if (test_name == "empty")
  {
    test_empty();
  }
  else if (test_name == "complete")
  {
    test_complete();
  }
  else if (test_name == "error")
  {
    test_error();
  }
  else
  {
    std::cerr << "Error: Unknown test: " << test_name << std::endl;
  }
}

void
test_empty()
{
  vistk::datum_t dat = vistk::datum::empty_datum();

  if (!dat->get_error().empty())
  {
    std::cerr << "Error: An empty datum has an error string" << std::endl;
  }

  bool got_exception = false;

  try
  {
    dat->get_datum<int>();
  }
  catch (vistk::bad_datum_cast_exception& e)
  {
    got_exception = true;

    (void)e.what();
  }
  catch (std::exception& e)
  {
    std::cerr << "Error: Unexpected exception: "
              << e.what() << std::endl;

    got_exception = true;
  }

  if (!got_exception)
  {
    std::cerr << "Error: Did not get expected exception "
              << "when retrieving a value from an empty datum" << std::endl;
  }
}

void
test_complete()
{
  vistk::datum_t dat = vistk::datum::complete_datum();

  if (!dat->get_error().empty())
  {
    std::cerr << "Error: A complete datum has an error string" << std::endl;
  }

  bool got_exception = false;

  try
  {
    dat->get_datum<int>();
  }
  catch (vistk::bad_datum_cast_exception& e)
  {
    got_exception = true;

    (void)e.what();
  }
  catch (std::exception& e)
  {
    std::cerr << "Error: Unexpected exception: "
              << e.what() << std::endl;

    got_exception = true;
  }

  if (!got_exception)
  {
    std::cerr << "Error: Did not get expected exception "
              << "when retrieving a value from a complete datum" << std::endl;
  }
}

void
test_error()
{
  vistk::datum::error_t const error = vistk::datum::error_t("An error");
  vistk::datum_t dat = vistk::datum::error_datum(error);

  if (dat->get_error() != error)
  {
    std::cerr << "Error: An error datum did not keep the message" << std::endl;
  }

  bool got_exception = false;

  try
  {
    dat->get_datum<int>();
  }
  catch (vistk::bad_datum_cast_exception& e)
  {
    got_exception = true;

    (void)e.what();
  }
  catch (std::exception& e)
  {
    std::cerr << "Error: Unexpected exception: "
              << e.what() << std::endl;

    got_exception = true;
  }

  if (!got_exception)
  {
    std::cerr << "Error: Did not get expected exception "
              << "when retrieving a value from an error datum" << std::endl;
  }
}
