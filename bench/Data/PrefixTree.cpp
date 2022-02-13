#include <Kal/Data/PrefixTree.hpp>

#include <hayai/hayai.hpp>
#include <hayai/hayai_fixture.hpp>

#include <fstream>

class PrefixTreeF : public ::hayai::Fixture {
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


  std::vector<std::string>      strings{ };

  size_t                        index = 0;
  Kal::Data::PrefixTree<size_t> tree{ };
};

class PrefixTreeFPre : public ::hayai::Fixture {
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
        tree.add (index++, s.c_str ( ));
      }
      in.close ( );
    }
    index = 0;
  }


  std::vector<std::string>      strings{ };

  size_t                        index = 0;
  Kal::Data::PrefixTree<size_t> tree{ };
};

BENCHMARK_F (PrefixTreeF, Add100, 10, 100) {
  tree.add (index, strings[index].c_str ( ));
  index++;
}

BENCHMARK_F (PrefixTreeF, Add50000, 10, 50000) {
  tree.add (index, strings[index].c_str ( ));
  index++;
}

BENCHMARK_F (PrefixTreeF, AddOnce, 10, 100) {
  tree.add (index, strings[index].c_str ( ));
}

BENCHMARK_F (PrefixTreeFPre, LookUp100, 10, 100) {
  tree.get (strings[index++].c_str ( ));
}

BENCHMARK_F (PrefixTreeFPre, LookUp50000, 10, 50000) {
  tree.get (strings[index++].c_str ( ));
}
