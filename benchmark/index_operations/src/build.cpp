#include <sdsl/suffix_arrays.hpp>
#include <iostream>
#include <string>

using namespace sdsl;
using namespace std;

int main(int argc, char** argv)
{
    if (argc < 5) {
        cout << "Usage ./" << argv[0] << " input_file tmp_dir output_file num_bytes" << endl;
        return 0;
    }
    auto input_file = argv[1];
    auto tmp_dir = argv[2];
    auto output_file = argv[3];
    auto num_bytes = std::atoi(argv[4]);

    if (num_bytes == 1) {
        CSA_TYPE csa;
        cache_config cconfig(false,tmp_dir,util::basename(input_file));
        construct(csa,input_file, cconfig,num_bytes);
        store_to_file(csa,output_file);
    }
    if (num_bytes != 1) {
        CSA_TYPE_INT csa;
        cache_config cconfig(false,tmp_dir,util::basename(input_file));
        construct(csa,input_file, cconfig,num_bytes);
        store_to_file(csa,output_file);
    }
}
