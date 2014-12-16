#include <sdsl/suffix_arrays.hpp>
#include <iostream>
#include <string>

using namespace sdsl;
using namespace std;

const uint32_t num_iters = 100000;
const uint32_t rnd_seed = 4711;

using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;

template<class t_index>
void measure_sa(const t_index& csa,const std::string& input,std::ofstream& out)
{
    // generate the positions we want to access
    std::mt19937 gen(rnd_seed);
    std::uniform_int_distribution<> pos_dis(0,csa.size()-1);
    std::vector<uint64_t> pos(num_iters);
    for (size_t i=0; i<num_iters; i++) pos[i] = pos_dis(gen);
    // measure access time
    size_t checksum = 0;
    auto start = timer::now();
    for (const auto& p : pos) {
        checksum += csa[p];
    }
    auto stop = timer::now();
    // compute space usage
    auto space_usage = sdsl::size_in_bytes(csa);
    auto text_size = csa.size();
    if (t_index::alphabet_type::int_width == 0) {
        text_size *= (sdsl::bits::hi(csa.sigma)+1);
        text_size = text_size/8;
    }

    out << input << ";"
        << NAME << ";"
        << t_index::sa_sample_dens << ";"
        << t_index::isa_sample_dens << ";"
        << "SA_ACCESS" << ";"
        << space_usage << ";"
        << text_size << ";"
        << num_iters << ";"
        << duration_cast<microseconds>(stop-start).count() << ";"
        << checksum << std::endl;
}

template<class t_index>
void measure_isa(const t_index& csa,const std::string& input,std::ofstream& out)
{
    // generate the positions we want to access
    std::mt19937 gen(rnd_seed);
    std::uniform_int_distribution<> pos_dis(0,csa.size()-1);
    std::vector<uint64_t> pos(num_iters);
    for (size_t i=0; i<num_iters; i++) pos[i] = pos_dis(gen);
    // measure access time
    size_t checksum = 0;
    auto start = timer::now();
    for (const auto& p : pos) {
        checksum += csa.isa[p];
    }
    auto stop = timer::now();
    // compute space usage
    auto space_usage = sdsl::size_in_bytes(csa);
    auto text_size = csa.size();
    if (t_index::alphabet_type::int_width == 0) {
        text_size *= (sdsl::bits::hi(csa.sigma)+1);
        text_size = text_size/8;
    }

    out << input << ";"
        << NAME << ";"
        << t_index::sa_sample_dens << ";"
        << t_index::isa_sample_dens << ";"
        << "ISA_ACCESS" << ";"
        << space_usage << ";"
        << text_size << ";"
        << num_iters << ";"
        << duration_cast<microseconds>(stop-start).count() << ";"
        << checksum << std::endl;
}

template<class t_index>
void measure_bwt(const t_index& csa,const std::string& input,std::ofstream& out)
{
    // generate the positions we want to access
    std::mt19937 gen(rnd_seed);
    std::uniform_int_distribution<> pos_dis(0,csa.size()-1);
    std::vector<uint64_t> pos(num_iters);
    for (size_t i=0; i<num_iters; i++) pos[i] = pos_dis(gen);
    // measure access time
    size_t checksum = 0;
    auto start = timer::now();
    for (const auto& p : pos) {
        checksum += csa.bwt[p];
    }
    auto stop = timer::now();
    // compute space usage
    auto space_usage = sdsl::size_in_bytes(csa);
    auto text_size = csa.size();
    if (t_index::alphabet_type::int_width == 0) {
        text_size *= (sdsl::bits::hi(csa.sigma)+1);
        text_size = text_size/8;
    }

    out << input << ";"
        << NAME << ";"
        << t_index::sa_sample_dens << ";"
        << t_index::isa_sample_dens << ";"
        << "BWT_ACCESS" << ";"
        << space_usage << ";"
        << text_size << ";"
        << num_iters << ";"
        << duration_cast<microseconds>(stop-start).count() << ";"
        << checksum << std::endl;
}

template<class t_index>
void measure_psi(const t_index& csa,const std::string& input,std::ofstream& out)
{
    // generate the positions we want to access
    std::mt19937 gen(rnd_seed);
    std::uniform_int_distribution<> pos_dis(0,csa.size()-1);
    std::vector<uint64_t> pos(num_iters);
    for (size_t i=0; i<num_iters; i++) pos[i] = pos_dis(gen);
    // measure access time
    size_t checksum = 0;
    auto start = timer::now();
    for (const auto& p : pos) {
        checksum += csa.psi[p];
    }
    auto stop = timer::now();
    // compute space usage
    auto space_usage = sdsl::size_in_bytes(csa);
    auto text_size = csa.size();
    if (t_index::alphabet_type::int_width == 0) {
        text_size *= (sdsl::bits::hi(csa.sigma)+1);
        text_size = text_size/8;
    }

    out << input << ";"
        << NAME << ";"
        << t_index::sa_sample_dens << ";"
        << t_index::isa_sample_dens << ";"
        << "PSI_ACCESS" << ";"
        << space_usage << ";"
        << text_size << ";"
        << num_iters << ";"
        << duration_cast<microseconds>(stop-start).count() << ";"
        << checksum << std::endl;
}

int main(int argc, char** argv)
{
    if (argc != 5) {
        return -1;
    }
    auto index_file = argv[1];
    auto input_file = argv[2];
    auto num_bytes = std::atoi(argv[3]);
    auto output_file = argv[4];
    std::ofstream res_stream(output_file,ios_base::app);

    if (num_bytes == 1) {
        CSA_TYPE csa;
        sdsl::load_from_file(csa,index_file);
        measure_sa(csa,input_file,res_stream);
        measure_isa(csa,input_file,res_stream);
        measure_bwt(csa,input_file,res_stream);
        measure_psi(csa,input_file,res_stream);
    } else {
        CSA_TYPE_INT csa;
        sdsl::load_from_file(csa,index_file);
        measure_sa(csa,input_file,res_stream);
        measure_isa(csa,input_file,res_stream);
        measure_bwt(csa,input_file,res_stream);
        measure_psi(csa,input_file,res_stream);
    }
}
