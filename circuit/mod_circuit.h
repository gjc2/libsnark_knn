
#include "add_k_circuit.h"
using namespace gadgetlib2;

CREATE_GADGET_BASE_CLASS(Mod_GadgetBase);

class R1P_Mod_Gadget: public Mod_GadgetBase, public R1P_Gadget{
    public:
        void generateConstraints();
        void generateWitness();

        friend class Mod_Gadget;
    
    private:
        R1P_Mod_Gadget(
            ProtoboardPtr pb,
            const Variable& input,
            const Variable& mod,
            const Variable& output,
            const Variable& rd
        );

        void init();

        const Variable input_;
        const Variable mod_;
        const Variable output_;
        const Variable rd_;

        VariableArray power2;// input = s * mod + r, s=(用二进制表达), power2表示bool(exist)2^n * 3)
        VariableArray mid_result;
        VariableArray mid_l;
        VariableArray mid_r;
        VariableArray less;
        VariableArray lessOrEqual;
        Variable temp;
        
        GadgetPtr comparisionGadget[32];
        GadgetPtr sumGadget;

        DISALLOW_COPY_AND_ASSIGN(R1P_Mod_Gadget);

};

CREATE_GADGET_FACTORY_CLASS_4(
    Mod_Gadget,
    Variable, input,
    Variable, mod,
    Variable, output,
    Variable, rd
)

Mod_GadgetBase::~Mod_GadgetBase(){}

R1P_Mod_Gadget::R1P_Mod_Gadget(
    ProtoboardPtr pb,
    const Variable& input,
    const Variable& mod,
    const Variable& output,
    const Variable& rd 
):Gadget(pb), Mod_GadgetBase(pb), R1P_Gadget(pb),
    input_(input), mod_(mod), output_(output),rd_(rd){}


void R1P_Mod_Gadget::init(){
    power2 = VariableArray(32, "power2_");
    mid_result = VariableArray(32, "mid_result");
    mid_l = VariableArray(32, "mid_l");
    mid_r = VariableArray(32, "mid_r");
    less = VariableArray(32, "less");
    lessOrEqual =VariableArray(32, "lessOrEqual");
    sumGadget = InnerProduct_Gadget::create(
        pb_,
        lessOrEqual,
        power2,
        temp
    );
    for(int i=32-1;i>=0;i--){
        if(i!=31){
            comparisionGadget[i]=Comparison_Gadget::create(
                pb_,
                32,
                power2[i],
                mid_result[i+1],
                less[i],
                lessOrEqual[i]
            );
        }else{
            comparisionGadget[i]=Comparison_Gadget::create(
                pb_,
                32,
                power2[i],
                input_,
                less[i],
                lessOrEqual[i]
            );
        }
    }
}

void R1P_Mod_Gadget::generateConstraints(){
    for(int i=0;i<32;i++){
        pb_->addRank1Constraint(
            1<<i,
            3,
            power2[i],
            "power2[i]=3*2^i"
        );
    }
    for(int i=32-1;i>=0;i--){
        if(i!=31){
            comparisionGadget[i]->generateConstraints();
            pb_->addRank1Constraint(
                mid_result[i+1]-power2[i],
                lessOrEqual[i],
                mid_l[i],
                "mid_l[i]"
            );
            pb_->addRank1Constraint(
                mid_result[i+1],
                1 - lessOrEqual[i],
                mid_r[i],
                "mid_r[i]"
            );
            pb_->addRank1Constraint(
                mid_l[i]+mid_r[i],
                1,
                mid_result[i],
                "l+r=all"
            );
        }else{
            comparisionGadget[i]->generateConstraints();
            pb_->addRank1Constraint(
                input_-power2[i],
                lessOrEqual[i],
                mid_l[i],
                "mid_l[i]"
            );
            pb_->addRank1Constraint(
                input_,
                1 - lessOrEqual[i],
                mid_r[i],
                "mid_r[i]"
            );
            pb_->addRank1Constraint(
                mid_l[i]+mid_r[i],
                1,
                mid_result[i],
                "l+r=all"
            );
        }

    }
    sumGadget->generateConstraints();
    pb_->addRank1Constraint(
        output_,
        mod_,
        temp,
        "output=temp/mod"
    );
    pb_->addRank1Constraint(
        mid_result[0],
        1,
        rd_,
        "rd=mid_result[0]"
    );
}

void R1P_Mod_Gadget::generateWitness(){
    for(int i=0;i<32;i++){
        val(power2[i])=val(1<<i)*val(3);
    }
    for(int i=32-1;i>=0;i--){
        if(i!=31){
            comparisionGadget[i]->generateWitness();
            val(mid_l[i])=(val(mid_result[i+1])-val(power2[i]))*val(lessOrEqual[i]);
            val(mid_r[i])=val(mid_result[i+1])*val(1-lessOrEqual[i]);
            val(mid_result[i])=val(mid_l[i])+val(mid_r[i]);
        }else{
            comparisionGadget[i]->generateWitness();
            val(mid_l[i])=(val(input_)-val(power2[i]))*val(lessOrEqual[i]);
            val(mid_r[i])=val(input_)*val(1-lessOrEqual[i]);
            val(mid_result[i])=val(mid_l[i])+val(mid_r[i]);
        }
    }
    sumGadget->generateWitness();
    val(output_)=val(temp)*val(mod_).inverse(fieldType());
    val(rd_)=val(mid_result[0]);



}