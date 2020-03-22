#include "registry/registry.h"

#include "gtest/gtest.h"

namespace registry {

class RegistryTest : public ::testing::Test {
 public:
  RegistryTest() {}
};

TEST_F(RegistryTest, ConstructDestructTest) {
  Registry registry("test_registry");
}

TEST_F(RegistryTest, AddChildRegistryTest) {
  Registry registry("test_registry");
  common::ErrorOr<Registry*> child1 = registry.AddChildRegistry("child1");
  ASSERT_TRUE(child1.HasValue());
  common::ErrorOr<Registry*> child2 = registry.AddChildRegistry("child2");
  ASSERT_TRUE(child2.HasValue());
  common::ErrorOr<Registry*> grandchild1 =
      child1.ValueOrDie()->AddChildRegistry("grandchild1");
  ASSERT_TRUE(grandchild1.HasValue());
}

TEST_F(RegistryTest, FindOrAddChildRegistryTest) {
  Registry registry("test_registry");
  common::ErrorOr<Registry*> child1 = registry.AddChildRegistry("child1");
  ASSERT_TRUE(child1.HasValue());
  Registry* child2 = registry.FindOrAddChildRegistry("child2");
  common::ErrorOr<Registry*> child2_copy = registry.FindChildRegistry("child2");
  ASSERT_TRUE(child2_copy.HasValue());
  EXPECT_EQ(child2, child2_copy.ValueOrDie());
}

TEST_F(RegistryTest, DuplicateAddChildRegistryTest) {
  Registry registry("test_registry");
  common::ErrorOr<Registry*> child1 = registry.AddChildRegistry("child1");
  ASSERT_TRUE(child1.HasValue());
  common::ErrorOr<Registry*> child1_copy = registry.AddChildRegistry("child1");
  EXPECT_FALSE(child1_copy.HasValue());
}

TEST_F(RegistryTest, AddBoolElementTest) {
  Registry registry("test_registry");
  common::ErrorOr<Registry*> child = registry.AddChildRegistry("child1");
  ASSERT_TRUE(child.ValueOrDie());
  common::ErrorOr<Registry::Bool*> test_bool = registry.AddBoolean("test_bool");
  ASSERT_TRUE(test_bool.HasValue());
  common::ErrorOr<Registry::Bool*> test_bool_copy =
      registry.AddBoolean("test_bool");
  EXPECT_FALSE(test_bool_copy.HasValue());

  common::ErrorOr<Registry::Bool*> test_child_bool =
      child.ValueOrDie()->AddBoolean("test&^_bool.");
  ASSERT_TRUE(test_child_bool.HasValue());
  EXPECT_NE(test_child_bool.ValueOrDie(), test_bool.ValueOrDie());

  common::ErrorOr<Registry::Element*> test_search =
      registry.FindElementByExtendedName("test_registry.child1.test_bool");
  EXPECT_TRUE(test_search.HasValue());

  test_search = registry.FindElementByExtendedName("child1.test_bool");
  EXPECT_TRUE(test_search.HasValue());
}

TEST_F(RegistryTest, ChildRegistryNamesTest) {
  Registry parent("parent_registry");
  parent.AddChildRegistry("child1");
  parent.AddChildRegistry("child2");
  parent.AddChildRegistry("child3");
  parent.AddChildRegistry("child4");
  std::set<std::string> names = parent.GetChildRegistryNames();
  EXPECT_EQ(names.count("child1"), 1);
  EXPECT_EQ(names.count("child2"), 1);
  EXPECT_EQ(names.count("child3"), 1);
  EXPECT_EQ(names.count("child4"), 1);
}

TEST(RegistryElementTest, ConstructDestructTest) {
  std::unique_ptr<Registry::Element> parameter_int32 =
      std::unique_ptr<Registry::Int32>(new Registry::Int32("test_int32", -1));

  std::unique_ptr<Registry::Element> parameter_uint32 =
      std::unique_ptr<Registry::UnsignedInt32>(
          new Registry::UnsignedInt32("test_uint32", 1));

  std::unique_ptr<Registry::Element> parameter_int64 =
      std::unique_ptr<Registry::Int64>(new Registry::Int64("test_int64", -1));

  std::unique_ptr<Registry::Element> parameter_uint64 =
      std::unique_ptr<Registry::UnsignedInt64>(
          new Registry::UnsignedInt64("test_uint64", 1));

  std::unique_ptr<Registry::Element> parameter_bool =
      std::unique_ptr<Registry::Bool>(new Registry::Bool("test_boolean", 1));

  std::unique_ptr<Registry::Element> parameter_char =
      std::unique_ptr<Registry::Char>(new Registry::Char("test_char", 'a'));

  std::unique_ptr<Registry::Element> parameter_string =
      std::unique_ptr<Registry::String>(
          new Registry::String("test_boolean", "string"));

  std::unique_ptr<Registry::Element> parameter_float =
      std::unique_ptr<Registry::Float>(
          new Registry::Float("test_double", 10.0f));

  std::unique_ptr<Registry::Element> parameter_double =
      std::unique_ptr<Registry::Double>(
          new Registry::Double("test_double", 1e1));
}

TEST(RegistryElementTest, TemplateMetaprogramTest) {
  std::unique_ptr<Registry::Element> parameter_int32 =
      std::unique_ptr<Registry::Int32>(new Registry::Int32("test_int32", -1));
  EXPECT_FALSE(parameter_int32->Assign('a'));
  int32_t int_value = 0;
  EXPECT_TRUE(parameter_int32->Extract(&int_value));
  EXPECT_EQ(int_value, -1);
  EXPECT_FALSE(parameter_int32->Assign(true));
  std::string str = "abc";
  EXPECT_FALSE(parameter_int32->Extract(&str));
  EXPECT_EQ(str, "abc");
  EXPECT_TRUE(parameter_int32->Assign(120));
  EXPECT_TRUE(parameter_int32->Extract(&int_value));
  EXPECT_EQ(int_value, 120);

  int32_t value = *(static_cast<Registry::Int32*>(parameter_int32.get()));
  EXPECT_EQ(value, 120);
}

TEST(RegistryElementTest, NumericalOperatorTest) {
  Registry::Int32 a("a", 1);
  Registry::Int32 b("b", 2);
  Registry::Int32 c("c", 3);
  a = b + c;
  EXPECT_EQ(a, 5);
}

}  // namespace registry
