#include <gtest/gtest.h>
#include "edit_distance.h"
#include "string"

using namespace std;

TEST(EditDistance, Empty) {
  ASSERT_EQ(0, EditDistance("", ""));
  ASSERT_EQ(3, EditDistance("any", ""));
  ASSERT_EQ(8, EditDistance("", "anything"));
}

TEST(EditDistance, Zero) {
  ASSERT_EQ(0, EditDistance("apple", "apple"));
  ASSERT_EQ(0, EditDistance("", ""));
  ASSERT_EQ(0, EditDistance("hello", "hello"));
}

TEST(EditDistance, OneSubstitution) {
  ASSERT_EQ(1, EditDistance("apple", "bpple"));
  ASSERT_EQ(1, EditDistance("apple", "abple"));
  ASSERT_EQ(1, EditDistance("apple", "apcle"));
  ASSERT_EQ(1, EditDistance("apple", "appce"));
  ASSERT_EQ(1, EditDistance("apple", "appld"));
  ASSERT_EQ(1, EditDistance("hello", "hallo"));
  ASSERT_EQ(1, EditDistance("world", "wdrld"));
  ASSERT_EQ(1, EditDistance("test", "best"));
}

TEST(EditDistance, OneDeletion) {
  ASSERT_EQ(1, EditDistance("apple", "pple"));
  ASSERT_EQ(1, EditDistance("apple", "aple"));
  ASSERT_EQ(1, EditDistance("apple", "aple"));
  ASSERT_EQ(1, EditDistance("apple", "appe"));
  ASSERT_EQ(1, EditDistance("apple", "appl"));
  ASSERT_EQ(1, EditDistance("hello", "hllo"));
  ASSERT_EQ(1, EditDistance("world", "wrld"));
  ASSERT_EQ(1, EditDistance("test", "est"));
}

TEST(EditDistance, OneAddition) {
  ASSERT_EQ(1, EditDistance("apple", "bapple"));
  ASSERT_EQ(1, EditDistance("apple", "acpple"));
  ASSERT_EQ(1, EditDistance("apple", "apdple"));
  ASSERT_EQ(1, EditDistance("apple", "appele"));
  ASSERT_EQ(1, EditDistance("apple", "applfe"));
  ASSERT_EQ(1, EditDistance("apple", "applex"));
  ASSERT_EQ(1, EditDistance("hello", "hellox"));
  ASSERT_EQ(1, EditDistance("stab", "stabX"));
}

TEST(EditDistance, OneAdjacentSwap) {
  ASSERT_EQ(2, EditDistance("apple", "aplpe"));
  ASSERT_EQ(2, EditDistance("apple", "appel"));
  ASSERT_EQ(2, EditDistance("appel", "apple"));
  ASSERT_EQ(2, EditDistance("apple", "paple"));
  ASSERT_EQ(2, EditDistance("hello", "ehllo"));
}

TEST(EditDistance, TwoSubstitution) {
  ASSERT_EQ(2, EditDistance("apple", "aqqle"));
  ASSERT_EQ(2, EditDistance("apple", "eppla"));
  ASSERT_EQ(2, EditDistance("apple", "appid"));
  ASSERT_EQ(2, EditDistance("hello", "haxlo"));
}

TEST(EditDistance, TwoDeletion) {
  ASSERT_EQ(2, EditDistance("apple", "ple"));
  ASSERT_EQ(2, EditDistance("apple", "ale"));
  ASSERT_EQ(2, EditDistance("apple", "ape"));
  ASSERT_EQ(2, EditDistance("apple", "app"));

  ASSERT_EQ(2, EditDistance("apple", "ppl"));
  ASSERT_EQ(2, EditDistance("apple", "ape"));
  ASSERT_EQ(2, EditDistance("apple", "ppe"));

  ASSERT_EQ(3, EditDistance("hello", "ho"));
  ASSERT_EQ(2, EditDistance("hello", "llo"));
}

TEST(EditDistance, TwoAddition) {
  ASSERT_EQ(2, EditDistance("apple", "adpplea"));
  ASSERT_EQ(2, EditDistance("apple", "abppble"));
  ASSERT_EQ(2, EditDistance("apple", "aprplre"));
  ASSERT_EQ(2, EditDistance("apple", "applrre"));
  ASSERT_EQ(2, EditDistance("apple", "applerr"));
  ASSERT_EQ(2, EditDistance("hello", "xhellox"));
  ASSERT_EQ(2, EditDistance("world", "wxorldx"));
}

TEST(EditDistance, ThreeSubstitution) {
  ASSERT_EQ(3, EditDistance("apple", "epaie"));
  ASSERT_EQ(3, EditDistance("apple", "dqqle"));
  ASSERT_EQ(3, EditDistance("apple", "abbpe"));
  ASSERT_EQ(3, EditDistance("hello", "xyzlo"));
}

TEST(EditDistance, ThreeDeletion) {
  ASSERT_EQ(3, EditDistance("apple", "ap"));
  ASSERT_EQ(3, EditDistance("apple", "pe"));
  ASSERT_EQ(3, EditDistance("apple", "pl"));
  ASSERT_EQ(3, EditDistance("apple", "pp"));
  ASSERT_EQ(4, EditDistance("beautiful", "beaut"));
}

TEST(EditDistance, ThreeAddition) {
  ASSERT_EQ(9, EditDistance("world", "beautifulworld"));
  ASSERT_EQ(8, EditDistance("test", "testworldxyz"));
}

TEST(EditDistance, Symmetric) {

  EXPECT_EQ(EditDistance("abc", "def"), EditDistance("def", "abc"));
  EXPECT_EQ(EditDistance("hello", "world"), EditDistance("world", "hello"));
  EXPECT_EQ(EditDistance("test", ""), EditDistance("", "test"));
}

TEST(EditDistance, Transitive) {

  EXPECT_EQ(1, EditDistance("cat", "bat"));
  EXPECT_EQ(1, EditDistance("bat", "bit"));
  EXPECT_EQ(1, EditDistance("cat", "cut"));
}

TEST(EditDistance, ComplexTransformations) {
  EXPECT_EQ(3, EditDistance("kitten", "sitting"));
  EXPECT_EQ(3, EditDistance("saturday", "sunday"));
  EXPECT_EQ(6, EditDistance("algorithm", "altruistic"));
}

TEST(EditDistance, SingleCharacterStrings) {
  EXPECT_EQ(0, EditDistance("a", "a"));
  EXPECT_EQ(1, EditDistance("a", "b"));
  EXPECT_EQ(1, EditDistance("a", ""));
  EXPECT_EQ(1, EditDistance("", "a"));
  EXPECT_EQ(2, EditDistance("a", "bc"));
  EXPECT_EQ(2, EditDistance("ab", "c"));
}

TEST(EditDistance, RepeatedCharacters) {
  EXPECT_EQ(0, EditDistance("aaa", "aaa"));
  EXPECT_EQ(1, EditDistance("aaa", "aab"));
  EXPECT_EQ(2, EditDistance("aaa", "abb"));
  EXPECT_EQ(3, EditDistance("aaa", "bbb"));
  EXPECT_EQ(2, EditDistance("aaaa", "aa"));
}

TEST(EditDistance, VeryLongStrings) {
  string str1(1000, 'a');
  string str2(1000, 'b');
  string str3(500, 'a');

  EXPECT_EQ(0, EditDistance(str1, str1));
  EXPECT_EQ(1000, EditDistance(str1, str2));
  EXPECT_EQ(500, EditDistance(str1, str3));
}

TEST(EditDistance, SpecialCharacters) {
  EXPECT_EQ(3, EditDistance("hello@world.com", "hello@world.org"));
  EXPECT_EQ(1, EditDistance("user_name", "user-name"));
  EXPECT_EQ(0, EditDistance("test123", "test123"));
  EXPECT_EQ(3, EditDistance("123", "456"));
}

TEST(EditDistance, ASCIICharacters) {

  EXPECT_EQ(0, EditDistance("cafe", "cafe"));
  EXPECT_EQ(1, EditDistance("cafe", "caf"));
}

TEST(EditDistance, AdjacentSwaps) {

  EXPECT_EQ(2, EditDistance("ab", "ba"));
  EXPECT_EQ(2, EditDistance("abc", "acb"));
  EXPECT_EQ(2, EditDistance("abc", "bac"));
  EXPECT_EQ(4, EditDistance("abcd", "dcba"));
}

TEST(EditDistance, InsertionsAndDeletions) {
  EXPECT_EQ(3, EditDistance("abc", "abcdef"));
  EXPECT_EQ(3, EditDistance("abcdef", "abc"));
  EXPECT_EQ(4, EditDistance("abcd", "efgh"));
  EXPECT_EQ(6, EditDistance("abc", "defghi"));
}

TEST(EditDistance, PrefixSuffix) {
  EXPECT_EQ(3, EditDistance("prefix", "fix"));
  EXPECT_EQ(3, EditDistance("test", "testing"));
  EXPECT_EQ(3, EditDistance("prefix", "suffix"));
}

TEST(EditDistance, MaximumDistance) {

  string str1 = "aaaaa";
  string str2 = "bbbbb";
  EXPECT_EQ(5, EditDistance(str1, str2));

  str1 = "hello";
  str2 = "world";
  EXPECT_EQ(4, EditDistance(str1, str2));
}

TEST(EditDistance, FirstLetterOfTargetWordIsLowerCase) {

  string str1 = "Hello";
  string str2 = "hello";
  EXPECT_EQ(1, EditDistance(str1, str2));

  str1 = "zHello";
  str2 = "hello";
  EXPECT_EQ(2, EditDistance(str1, str2));

  str1 = "Helo";
  str2 = "hello";
  EXPECT_EQ(2, EditDistance(str1, str2));

  str1 = "Hella";
  str2 = "hello";
  EXPECT_EQ(2, EditDistance(str1, str2));

  str1 = "hello";
  str2 = "Hello";
  EXPECT_EQ(1, EditDistance(str1, str2));

  str1 = "hell";
  str2 = "Hello";
  EXPECT_EQ(2, EditDistance(str1, str2));

  str1 = "helloyo";
  str2 = "Hello";
  EXPECT_EQ(3, EditDistance(str1, str2));
}
