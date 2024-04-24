#define CURVE_BN128
#include <libff/common/default_types/ec_pp.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_gg_ppzksnark/r1cs_gg_ppzksnark.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>
#include <libsnark/relations/constraint_satisfaction_problems/r1cs/examples/r1cs_examples.hpp>
#include <libsnark/gadgetlib2/gadget.hpp>
#include <libsnark/gadgetlib2/adapters.hpp>
#include <libsnark/gadgetlib2/integration.hpp>
#include "../circuit/knn_circuit.h"
#include <depends/gtest/googletest/include/gtest/gtest.h>
#include <iostream>
#include <fstream>
#include <string>


using namespace libsnark;
using namespace std;
using namespace gadgetlib2;
using namespace libff;



r1cs_example<libff::Fr<libff::default_ec_pp> > knn_example(){
    
    typedef libff::Fr<libff::default_ec_pp> FieldT;

    gadgetlib2::initPublicParamsFromDefaultPp();
    gadgetlib2::GadgetLibAdapter::resetVariableIndex();
    
    auto pb = Protoboard::create(gadgetlib2::R1P);

    
    std::vector<VariableArray>dataset;
    
    
    for(int i=0;i<LENGTH;i++){
        dataset.push_back(VariableArray(DATA_LENGTH, "data_"+std::to_string(i)));
    }

    const VariableArray label(LENGTH, "label");
    const VariableArray query(DATA_LENGTH, "query");
    const Variable k;
    const Variable output;
    
    
    auto knn = Knn_Gadget::create(
        pb,
        dataset,
        label,
        query,
        output,
        k
    );
    
    knn->generateConstraints();
    
    std::ifstream file("knn/dataset.txt");
    if(!file){
        std::cerr << "无法打开文件！" << std::endl;
        exit(0);
    }
    int temp;
    for(int i=0;i<LENGTH;i++){
        for(int j=0;j<DATA_LENGTH;j++){
            if(file >> temp){
                //std::cout<<temp<<std::endl;
                pb->val(dataset[i][j]) = temp;
            }else{
                std::cerr << "读取文件时发生错误或文件数据不足！" << std::endl;
                exit(0);
            }
        }
    }
    for(int i=0;i<LENGTH;i++){
        if(file >> temp){
            pb->val(label[i]) = temp;
        }else{
            std::cerr << "读取文件时发生错误或文件数据不足！" << std::endl;
            exit(0);
        }
    }
    for(int i=0;i<DATA_LENGTH;i++){
        if(file >> temp){
            pb->val(query[i]) = temp;
        }else{
            std::cerr << "读取文件时发生错误或文件数据不足！" << std::endl;
            exit(0);
        }
    }
    if(file >> temp){
        pb->val(k) = temp;
    }else{
        std::cerr << "读取文件时发生错误或文件数据不足！" << std::endl;
        exit(0);
    }
    
    knn->generateWitness();
    

    
    
    
    

    EXPECT_TRUE(pb->isSatisfied());
    auto a = pb->val(7);
    auto b = pb->val(3);
    auto expect = a*b.inverse(FieldType());
    auto test = b-a;
    EXPECT_EQ(pb->val(output), expect);
    EXPECT_EQ(pb->val(-4),test);
    EXPECT_EQ(3,4);
    
    
    // translate constraint system to libsnark format.
    r1cs_constraint_system<FieldT> cs = libsnark::get_constraint_system_from_gadgetlib2(*pb);
    
    // translate full variable assignment to libsnark format
    const r1cs_variable_assignment<FieldT> full_assignment = libsnark::get_variable_assignment_from_gadgetlib2(*pb);
    
    // extract primary and auxiliary input
    
    const r1cs_primary_input<FieldT> primary_input(full_assignment.begin(),full_assignment.begin()+cs.num_inputs());
    const r1cs_auxiliary_input<FieldT> auxiliary_input(full_assignment.begin()+cs.num_inputs(), full_assignment.end());
    
    assert(cs.is_valid());
    assert(cs.is_satisfied(primary_input, auxiliary_input));
    
    return r1cs_example<FieldT>(cs, primary_input, auxiliary_input);

}

void mod_example(){
    gadgetlib2::initPublicParamsFromDefaultPp();
    gadgetlib2::GadgetLibAdapter::resetVariableIndex();
    
    auto pb = Protoboard::create(gadgetlib2::R1P);
    const Variable p,q,s,r;
    auto mod = Mod_Gadget::create(
        pb,
        p,q,s,r
    );
    mod->generateConstraints();
    pb->val(p)=7;
    pb->val(q)=3;
    mod->generateWitness();

    EXPECT_EQ(pb->val(s),2);
    EXPECT_EQ(pb->val(r),1);

    
    
}
//Groth16
template<class ppT>
void run_r1cs_gg_ppzksnark(const r1cs_example<libff::Fr<ppT> >&example/*,const bool test_serialization*/){

    
    libff::enter_block("Call to run_r1cs_gg_ppzksnark");
    //Generator
    libff::print_header("R1CS gg_ppzkSNARK Generator");

    r1cs_gg_ppzksnark_keypair<ppT> keypair = r1cs_gg_ppzksnark_generator<ppT>(example.constraint_system);

    libff::print_mem("after generator");
    
    //Preprocess verification key
    libff::print_header("Preprocess verification key");
    r1cs_gg_ppzksnark_processed_verification_key<ppT> pvk = r1cs_gg_ppzksnark_verifier_process_vk<ppT>(keypair.vk);
    //Prover
    libff::print_header("R1CS gg_ppzkSNARK Prover");
    r1cs_gg_ppzksnark_proof<ppT> proof = r1cs_gg_ppzksnark_prover<ppT>(keypair.pk,example.primary_input,example.auxiliary_input);
    
    //verifier
    libff::print_header("R1CS gg_ppzkSNARK Verifier");
    const bool ans = r1cs_gg_ppzksnark_verifier_strong_IC<ppT>(keypair.vk, example.primary_input, proof);

    printf("* The verification result is: %s\n", (ans ? "PASS" : "FAIL"));

    const bool ans2 = r1cs_gg_ppzksnark_online_verifier_strong_IC<ppT>(pvk, example.primary_input, proof);

    assert(ans == ans2);
    
    
}
//BCTV14
template<class ppT>
void run_r1cs_ppzksnark(const r1cs_example<libff::Fr<ppT> >&example/*,const bool test_serialization*/){

    libff::enter_block("Call to run_r1cs_ppzksnark");
    //Generator
    libff::print_header("R1CS gg_ppzkSNARK Generator");

    r1cs_ppzksnark_keypair<ppT> keypair = r1cs_ppzksnark_generator<ppT>(example.constraint_system);

    libff::print_mem("after generator");
    
    //Preprocess verification key
    libff::print_header("Preprocess verification key");
    r1cs_ppzksnark_processed_verification_key<ppT> pvk = r1cs_ppzksnark_verifier_process_vk<ppT>(keypair.vk);
    //Prover
    libff::print_header("R1CS ppzkSNARK Prover");
    r1cs_ppzksnark_proof<ppT> proof = r1cs_ppzksnark_prover<ppT>(keypair.pk,example.primary_input,example.auxiliary_input);
    
    //verifier
    libff::print_header("R1CS ppzkSNARK Verifier");
    const bool ans = r1cs_ppzksnark_verifier_strong_IC<ppT>(keypair.vk, example.primary_input, proof);

    printf("* The verification result is: %s\n", (ans ? "PASS" : "FAIL"));

    const bool ans2 = r1cs_ppzksnark_online_verifier_strong_IC<ppT>(pvk, example.primary_input, proof);

    assert(ans == ans2);
    
    
}

int main(int argc, char **argv){
    // compare_example();
    // swap_example();
    // sorted_example();
    // andk_example();
    

    mod_example();
    // auto example = knn_example();
    // run_r1cs_gg_ppzksnark<default_r1cs_gg_ppzksnark_pp>(example);
    // run_r1cs_ppzksnark<default_r1cs_gg_ppzksnark_pp>(example);
}

