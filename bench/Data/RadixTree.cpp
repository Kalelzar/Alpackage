#include <Kal/Data/RadixTree.hpp>

#include <hayai/hayai.hpp>
#include <hayai/hayai_fixture.hpp>

#include <fstream>

class RadixTreeF : public ::hayai::Fixture {
  public:
  void TearDown ( ) override {
    strings.clear ( );
    tree.clear ( );
  }
  void SetUp ( ) override {
    std::ifstream in ("benchmarks/strings.txt");
    index = 0;
    if (in.is_open ( )) {
      while (!in.eof ( )) {
        std::string s;
        getline (in, s);
        if (in.fail ( )) break;
        strings.push_back (s);
      }
      in.close ( );
    }
  }


  std::vector<std::string>     strings{ };

  size_t                       index = 0;
  Kal::Data::RadixTree<size_t> tree{ };
};

class RadixTreeFPre : public ::hayai::Fixture {
  public:
  void TearDown ( ) override {
    strings.clear ( );
    tree.clear ( );
  }
  void SetUp ( ) override {
    std::ifstream in ("benchmarks/strings.txt");
    index = 0;
    if (in.is_open ( )) {
      while (!in.eof ( )) {
        std::string s;
        getline (in, s);
        if (in.fail ( )) break;
        strings.push_back (s);
        tree.add (s, index++);
      }
      in.close ( );
    }
    index = 0;
  }


  std::vector<std::string>     strings{ };

  size_t                       index = 0;
  Kal::Data::RadixTree<size_t> tree{ };
};

BENCHMARK_F (RadixTreeF, Add100, 10, 100) {
  tree.add (strings[index], index);
  index++;
}

BENCHMARK_F (RadixTreeF, Add50000, 10, 50000) {
  tree.add (strings[index], index);
  index++;
}

BENCHMARK_F (RadixTreeF, AddOnce, 10, 100) { tree.add (strings[index], index); }

BENCHMARK_F (RadixTreeFPre, LookUp100, 10, 100) {
  tree.lookup (strings[index++]);
}

BENCHMARK_F (RadixTreeFPre, LookUp50000, 10, 50000) {
  tree.lookup (strings[index++]);
}

BENCHMARK_F (RadixTreeFPre, Remove100, 10, 100) {
  tree.remove (strings[index++]);
}

BENCHMARK_F (RadixTreeFPre, Remove50000, 1, 10000) {
  tree.remove (strings[index++]);
}
