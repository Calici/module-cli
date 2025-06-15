#include <moderna/test_lib.hpp>
#include <rfl.hpp>
import pharmaco.core.type_schema;
import moderna.test_lib;

namespace core = pharmaco::core;
namespace test_lib = moderna::test_lib;

MODERNA_ADD_TEST(basic_type_schema_validation) {
  using namespace core;
  auto int_schema = type_schema{basic_type_schema::make("int")};
  test_lib::assert_expected(int_schema.validate_parameters(42));
  test_lib::assert_false(int_schema.validate_parameters("not-an-int").has_value());

  auto bool_schema = type_schema{basic_type_schema::make("bool")};
  test_lib::assert_expected(bool_schema.validate_parameters(true));
  test_lib::assert_false(bool_schema.validate_parameters(0.5).has_value());

  auto str_schema = type_schema{basic_type_schema::make("str")};
  test_lib::assert_expected(str_schema.validate_parameters("hello"));
  test_lib::assert_false(str_schema.validate_parameters(123).has_value());
}

MODERNA_ADD_TEST(literal_type_schema_validation) {
  using namespace core;
  type_schema wrapper{literal_type_schema::make("apple", "banana")};
  test_lib::assert_expected(wrapper.validate_parameters("apple"));
  test_lib::assert_expected(wrapper.validate_parameters("banana"));
  test_lib::assert_false(wrapper.validate_parameters("pear").has_value());
  test_lib::assert_false(wrapper.validate_parameters(5).has_value());
}

MODERNA_ADD_TEST(list_type_schema_validation) {
  using namespace core;
  type_schema wrapper{list_type_schema::make(basic_type_schema::make("int"))};

  rfl::Generic::Array good_list = {1, 2, 3};
  rfl::Generic::Array bad_list = {1, "bad", 3};

  test_lib::assert_expected(wrapper.validate_parameters(good_list));
  test_lib::assert_false(wrapper.validate_parameters(bad_list).has_value());
  test_lib::assert_false(wrapper.validate_parameters("not-a-list").has_value());
}

MODERNA_ADD_TEST(record_type_schema_validation) {
  using namespace core;
  type_schema wrapper{record_type_schema::make(basic_type_schema::make("str"))};

  rfl::Generic::Object good;
  good["a"] = "x";
  good["b"] = "y";
  rfl::Generic::Object bad;
  bad["a"] = "x";
  bad["b"] = 2;

  test_lib::assert_expected(wrapper.validate_parameters(good));
  test_lib::assert_false(wrapper.validate_parameters(bad).has_value());
}

MODERNA_ADD_TEST(typed_record_type_schema_validation) {
  using namespace core;
  type_schema wrapper{typed_record_type_schema::make()
                        .add("id", basic_type_schema::make("int"))
                        .add("name", basic_type_schema::make("str"))};

  rfl::Generic::Object good;
  good["id"] = 100;
  good["name"] = "Alice";
  rfl::Generic::Object missing_field;
  missing_field["id"] = 100;
  rfl::Generic::Object wrong_type;
  wrong_type["id"] = "bad";
  wrong_type["name"] = "Alice";

  test_lib::assert_expected(wrapper.validate_parameters(good));
  test_lib::assert_false(wrapper.validate_parameters(missing_field).has_value());
  test_lib::assert_false(wrapper.validate_parameters(wrong_type).has_value());
}

MODERNA_ADD_TEST(union_type_schema_validation) {
  using namespace core;
  type_schema wrapper{union_type_schema::make()
                        .add(basic_type_schema::make("int"))
                        .add(basic_type_schema::make("str"))};

  test_lib::assert_expected(wrapper.validate_parameters(123));
  test_lib::assert_expected(wrapper.validate_parameters("union string"));
  test_lib::assert_false(wrapper.validate_parameters(true).has_value());
}

MODERNA_ADD_TEST(nested_type_schema_validation) {
  using namespace core;
  type_schema wrapper{typed_record_type_schema::make()
                        .add("numbers", list_type_schema::make(basic_type_schema::make("int")))
                        .add("label", basic_type_schema::make("str"))};

  rfl::Generic::Object valid;
  valid["numbers"] = rfl::Generic::Array{1, 2, 3};
  valid["label"] = "group A";
  rfl::Generic::Object invalid;
  invalid["numbers"] = rfl::Generic::Array{1, "oops"};
  invalid["label"] = "group A";

  test_lib::assert_expected(wrapper.validate_parameters(valid));
  test_lib::assert_false(wrapper.validate_parameters(invalid).has_value());
}
