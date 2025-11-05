# Project 2 Skeleton with A Single Node Solution

## Usage

### Make
 
#### Spellcheck

```
$ make spellcheck \
    && ./build/spellcheck files/dictionary.spec.txt files/words.spec.txt \
    && echo "=======" \
    && cat results/word_list_misspelled.txt \
    && echo "=======" \
    && md5sum results/word_list_misspelled.txt
mpiCC  -std=c++17 -Iinclude  -O3 -c src/dictionary.cc -o build/dictionary.o
mpiCC  -std=c++17 -Iinclude  -O3 -c src/edit_distance.cc -o build/edit_distance.o
mpiCC  -std=c++17 -Iinclude  -O3 -c src/bk_tree.cc -o build/bk_tree.o
mpiCC  -std=c++17 -Iinclude  -O3 -c src/candidates.cc -o build/candidates.o
mpiCC  -std=c++17 -Iinclude  -O3 -c src/load_files.cc -o build/load_files.o
mpiCC -lmpi -fopenmp -lstdc++ build/dictionary.o build/edit_distance.o build/bk_tree.o build/candidates.o build/load_files.o spellcheck.cc -o build/spellcheck
[*] Using dictionary: files/dictionary.spec.txt
[*] Using word list: files/words.spec.txt
[*] Building dictionary with bk trees
        Dictionary size: 25
        Word list size: 3
        Root len: 5
[*] Getting misspelled words
[*] Number of threads: 32
[*] Output: results/word_list_misspelled.txt
[*] Wall time: 0 mins 0 secs 0 ms; 0
=======
appel: Rappel appeal rappel
tim: Tim Vim time vim
stap: sap snap stab step stop strap tap
=======
d2827f261f451f8eb205822b8addcbc9  results/word_list_misspelled.txt
```

#### Test

```
$ make test
mpiCC -std=c++17 -Iinclude  -O3 -Igtest/include -Igtest -c gtest/src/gtest-all.cc -o build/gtest/gtest-all.o
mpiCC -std=c++17 -Iinclude  -O3 -Igtest/include -Igtest -Igmock/include -Igmock -c gmock/src/gmock-all.cc -o build/gmock/gmock-all.o
g++ -std=c++17 -Iinclude  -O3 -Igtest/include -Igtest -Igmock/include -Igmock test/test_main.cc build/gtest/gtest-all.o build/gmock/gmock-all.o build/dictionary.o build/edit_distance.o build/bk_tree.o build/candidates.o build/load_files.o -lmpi -fopenmp -o build/test
build/test
[==========] Running 74 tests from 6 test suites.
[----------] Global test environment set-up.
[----------] 13 tests from BKTree
[ RUN      ] BKTree.GetCandidates
[       OK ] BKTree.GetCandidates (0 ms)
[ RUN      ] BKTree.EmptyTree
[       OK ] BKTree.EmptyTree (0 ms)
[ RUN      ] BKTree.SingleNode
[       OK ] BKTree.SingleNode (0 ms)
[ RUN      ] BKTree.ToleranceZero
[       OK ] BKTree.ToleranceZero (0 ms)
[ RUN      ] BKTree.ToleranceOne
[       OK ] BKTree.ToleranceOne (0 ms)
[ RUN      ] BKTree.DuplicateNodes
[       OK ] BKTree.DuplicateNodes (0 ms)
[ RUN      ] BKTree.VaryingWordLengths
[       OK ] BKTree.VaryingWordLengths (0 ms)
[ RUN      ] BKTree.LargeEditDistance
[       OK ] BKTree.LargeEditDistance (0 ms)
[ RUN      ] BKTree.CaseSensitive
[       OK ] BKTree.CaseSensitive (0 ms)
[ RUN      ] BKTree.SpecialCharacters
[       OK ] BKTree.SpecialCharacters (0 ms)
[ RUN      ] BKTree.EmptySearchString
[       OK ] BKTree.EmptySearchString (0 ms)
[ RUN      ] BKTree.ForceDistanceExecution
[       OK ] BKTree.ForceDistanceExecution (0 ms)
[ RUN      ] BKTree.PrintTreeCoverage
[       OK ] BKTree.PrintTreeCoverage (0 ms)
[----------] 13 tests from BKTree (0 ms total)

[----------] 13 tests from Candidates
[ RUN      ] Candidates.GetMisspelled
[*] Number of threads: 32
[       OK ] Candidates.GetMisspelled (0 ms)
[ RUN      ] Candidates.EmptyWordList
[*] Number of threads: 32
[       OK ] Candidates.EmptyWordList (0 ms)
[ RUN      ] Candidates.EmptyDictionary
[       OK ] Candidates.EmptyDictionary (0 ms)
[ RUN      ] Candidates.AllWordsCorrect
[*] Number of threads: 32
[       OK ] Candidates.AllWordsCorrect (0 ms)
[ RUN      ] Candidates.AllWordsMisspelled
[*] Number of threads: 32
[       OK ] Candidates.AllWordsMisspelled (0 ms)
[ RUN      ] Candidates.SingleCharacterWords
[*] Number of threads: 32
[       OK ] Candidates.SingleCharacterWords (0 ms)
[ RUN      ] Candidates.CaseSensitiveMatching
[*] Number of threads: 32
[       OK ] Candidates.CaseSensitiveMatching (0 ms)
[ RUN      ] Candidates.DuplicateWords
[*] Number of threads: 32
[       OK ] Candidates.DuplicateWords (0 ms)
[ RUN      ] Candidates.VeryLongWords
[*] Number of threads: 32
[       OK ] Candidates.VeryLongWords (0 ms)
[ RUN      ] Candidates.SpecialCharacters
[*] Number of threads: 32
[       OK ] Candidates.SpecialCharacters (0 ms)
[ RUN      ] Candidates.BuildWordListRootLength
[       OK ] Candidates.BuildWordListRootLength (0 ms)
[ RUN      ] Candidates.BuildWordListEmptyLines
[       OK ] Candidates.BuildWordListEmptyLines (0 ms)
[ RUN      ] Candidates.LowercaseDictionaryFirstLetter
[*] Number of threads: 32
[       OK ] Candidates.LowercaseDictionaryFirstLetter (0 ms)
[----------] 13 tests from Candidates (0 ms total)

[----------] 1 test from FileIOTest
[ RUN      ] FileIOTest.SaveMisspelled
[*] Number of threads: 32
[       OK ] FileIOTest.SaveMisspelled (0 ms)
[----------] 1 test from FileIOTest (0 ms total)

[----------] 20 tests from Dictionary
[ RUN      ] Dictionary.ContainsExactWord
[       OK ] Dictionary.ContainsExactWord (0 ms)
[ RUN      ] Dictionary.NotContainExactWord
[       OK ] Dictionary.NotContainExactWord (0 ms)
[ RUN      ] Dictionary.ContainsFirstLetterLowercase
[       OK ] Dictionary.ContainsFirstLetterLowercase (0 ms)
[ RUN      ] Dictionary.NotContainsFirstLetterLowercase
[       OK ] Dictionary.NotContainsFirstLetterLowercase (0 ms)
[ RUN      ] Dictionary.EmptyDictionary
[       OK ] Dictionary.EmptyDictionary (0 ms)
[ RUN      ] Dictionary.SingleWord
[       OK ] Dictionary.SingleWord (0 ms)
[ RUN      ] Dictionary.SingleCharacterWords
[       OK ] Dictionary.SingleCharacterWords (0 ms)
[ RUN      ] Dictionary.DuplicateWords
[       OK ] Dictionary.DuplicateWords (0 ms)
[ RUN      ] Dictionary.WordsWithNumbers
[       OK ] Dictionary.WordsWithNumbers (0 ms)
[ RUN      ] Dictionary.SpecialCharacters
[       OK ] Dictionary.SpecialCharacters (0 ms)
[ RUN      ] Dictionary.EmptyLines
[       OK ] Dictionary.EmptyLines (0 ms)
[ RUN      ] Dictionary.VeryLongWords
[       OK ] Dictionary.VeryLongWords (0 ms)
[ RUN      ] Dictionary.BuildBKTreeEmptyDictionary
[       OK ] Dictionary.BuildBKTreeEmptyDictionary (0 ms)
[ RUN      ] Dictionary.BuildBKTreeMissingLength
[       OK ] Dictionary.BuildBKTreeMissingLength (0 ms)
[ RUN      ] Dictionary.CandidatesSearchBasic
[       OK ] Dictionary.CandidatesSearchBasic (0 ms)
[ RUN      ] Dictionary.CaseSensitivityEdgeCases
[       OK ] Dictionary.CaseSensitivityEdgeCases (0 ms)
[ RUN      ] Dictionary.UppercaseHandling
[       OK ] Dictionary.UppercaseHandling (0 ms)
[ RUN      ] Dictionary.BuildBKTreeCornerCase
[       OK ] Dictionary.BuildBKTreeCornerCase (0 ms)
[ RUN      ] Dictionary.BuildBKTreeMultipleScenarios
[       OK ] Dictionary.BuildBKTreeMultipleScenarios (0 ms)
[ RUN      ] Dictionary.BuildBKTreeStressTest
[       OK ] Dictionary.BuildBKTreeStressTest (0 ms)
[----------] 20 tests from Dictionary (0 ms total)

[----------] 25 tests from EditDistance
[ RUN      ] EditDistance.Empty
[       OK ] EditDistance.Empty (0 ms)
[ RUN      ] EditDistance.Zero
[       OK ] EditDistance.Zero (0 ms)
[ RUN      ] EditDistance.OneSubstitution
[       OK ] EditDistance.OneSubstitution (0 ms)
[ RUN      ] EditDistance.OneDeletion
[       OK ] EditDistance.OneDeletion (0 ms)
[ RUN      ] EditDistance.OneAddition
[       OK ] EditDistance.OneAddition (0 ms)
[ RUN      ] EditDistance.OneAdjacentSwap
[       OK ] EditDistance.OneAdjacentSwap (0 ms)
[ RUN      ] EditDistance.TwoSubstitution
[       OK ] EditDistance.TwoSubstitution (0 ms)
[ RUN      ] EditDistance.TwoDeletion
[       OK ] EditDistance.TwoDeletion (0 ms)
[ RUN      ] EditDistance.TwoAddition
[       OK ] EditDistance.TwoAddition (0 ms)
[ RUN      ] EditDistance.ThreeSubstitution
[       OK ] EditDistance.ThreeSubstitution (0 ms)
[ RUN      ] EditDistance.ThreeDeletion
[       OK ] EditDistance.ThreeDeletion (0 ms)
[ RUN      ] EditDistance.ThreeAddition
[       OK ] EditDistance.ThreeAddition (0 ms)
[ RUN      ] EditDistance.Symmetric
[       OK ] EditDistance.Symmetric (0 ms)
[ RUN      ] EditDistance.Transitive
[       OK ] EditDistance.Transitive (0 ms)
[ RUN      ] EditDistance.ComplexTransformations
[       OK ] EditDistance.ComplexTransformations (0 ms)
[ RUN      ] EditDistance.SingleCharacterStrings
[       OK ] EditDistance.SingleCharacterStrings (0 ms)
[ RUN      ] EditDistance.RepeatedCharacters
[       OK ] EditDistance.RepeatedCharacters (0 ms)
[ RUN      ] EditDistance.VeryLongStrings
[       OK ] EditDistance.VeryLongStrings (17 ms)
[ RUN      ] EditDistance.SpecialCharacters
[       OK ] EditDistance.SpecialCharacters (0 ms)
[ RUN      ] EditDistance.ASCIICharacters
[       OK ] EditDistance.ASCIICharacters (0 ms)
[ RUN      ] EditDistance.AdjacentSwaps
[       OK ] EditDistance.AdjacentSwaps (0 ms)
[ RUN      ] EditDistance.InsertionsAndDeletions
[       OK ] EditDistance.InsertionsAndDeletions (0 ms)
[ RUN      ] EditDistance.PrefixSuffix
[       OK ] EditDistance.PrefixSuffix (0 ms)
[ RUN      ] EditDistance.MaximumDistance
[       OK ] EditDistance.MaximumDistance (0 ms)
[ RUN      ] EditDistance.FirstLetterOfTargetWordIsLowerCase
[       OK ] EditDistance.FirstLetterOfTargetWordIsLowerCase (0 ms)
[----------] 25 tests from EditDistance (17 ms total)

[----------] 2 tests from LoadFilesTest
[ RUN      ] LoadFilesTest.LoadValidFile
[       OK ] LoadFilesTest.LoadValidFile (0 ms)
[ RUN      ] LoadFilesTest.FileSize
[       OK ] LoadFilesTest.FileSize (0 ms)
[----------] 2 tests from LoadFilesTest (0 ms total)

[----------] Global test environment tear-down
[==========] 74 tests from 6 test suites ran. (19 ms total)
[  PASSED  ] 74 tests.
```

### CMake

#### Spellcheck

```
$ mkdir -p build && cd build && cmake .. && cmake --build . && ./spellcheck

Usage: ./build/spellcheck <dictionary> <word_list>
```

#### Test


```
$ mkdir -p build && cd build && cmake .. && cmake --build . && ./test

```