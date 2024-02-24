#include <libsnark/common/default_types/r1cs_gg_ppzksnark_pp.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_gg_ppzksnark/r1cs_gg_ppzksnark.hpp>
#include <libsnark/gadgetlib2/gadget.hpp>
#include <vector>
#include "sort_circuit.h"

using namespace gadgetlib2;
using namespace libsnark;


CREATE_GADGET_BASE_CLASS(andK_GadgetBase);

class R1P_andK_Gadget: public andK_GadgetBase, public R1P_Gadget{
    public:
        void generateConstraints();
        void generateWitness();

        friend class andK_Gadget;
    
    private:
        R1P_andK_Gadget(
            ProtoboardPtr pb,
            const VariableArray& inputs,
            const Variable& k,
            const Variable& output
        );

        void init();

        const VariableArray inputs_;
        const Variable k_;
        const Variable output_;

        
        const VariableArray sum = VariableArray(LENGTH+1, "sum_");
        const Variable successFlag;

        GadgetPtr chooseGadget;

        DISALLOW_COPY_AND_ASSIGN(R1P_andK_Gadget);
};

CREATE_GADGET_FACTORY_CLASS_3(
    andK_Gadget,
    VariableArray, inputs,
    Variable, k,
    Variable, output
)

andK_GadgetBase::~andK_GadgetBase(){}

R1P_andK_Gadget::R1P_andK_Gadget(
    ProtoboardPtr pb,
    const VariableArray& inputs,
    const Variable& k,
    const Variable& output
): Gadget(pb), R1P_Gadget(pb), andK_GadgetBase(pb),
    inputs_(inputs), k_(k), output_(output) {}

void R1P_andK_Gadget::init(){
    chooseGadget = LooseMUX_Gadget::create(
        pb_,
        sum,
        k_,
        output_,
        successFlag
    );
}

//sum[0]=0
//sum[1]=input[0]
//sum[i]=input[0]+...+input[i-1]
void R1P_andK_Gadget::generateConstraints(){
    pb_->addRank1Constraint(
        sum[0],
        1,
        0,
        "sum[0]=0"
    );

    for(int i=1;i<LENGTH+1;i++){
        pb_->addRank1Constraint(
            sum[i],
            1,
            sum[i-1]+inputs_[i-1],
            "sum[i] = sum[i-1]+input[i-1]"
        );
    }

    // pb_->addGeneralConstraint(
    //     successFlag,
    //     1,
    //     "successFlag = 1"
    // );
    chooseGadget->generateConstraints();
}

void R1P_andK_Gadget::generateWitness(){
    val(sum[0]) = 0;
    for(int i=1;i<LENGTH+1;i++){
        val(sum[i]) = val(sum[i-1]) + val(inputs_[i-1]);
    }
    // val(successFlag) = 1;
    chooseGadget->generateWitness();
}