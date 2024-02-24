#include <libsnark/common/default_types/r1cs_gg_ppzksnark_pp.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_gg_ppzksnark/r1cs_gg_ppzksnark.hpp>
#include <libsnark/gadgetlib2/gadget.hpp>
#include <vector>
using namespace gadgetlib2;

CREATE_GADGET_BASE_CLASS(Swap_GadgetBase);

class R1P_Swap_Gadget: public Swap_GadgetBase, public R1P_Gadget{
    public:
        void generateConstraints();
        void generateWitness();

        friend class Swap_Gadget;

    private:
        R1P_Swap_Gadget(
            ProtoboardPtr pb,
            const Variable& l,
            const Variable& r,
            const Variable& lmin,
            const Variable& rmax,
            const Variable& l_label,
            const Variable& r_label,
            const Variable& lmin_label,
            const Variable& rmax_label
        );

        void init();

        const Variable l_;
        const Variable r_;
        const Variable lmin_;
        const Variable rmax_;
        const Variable l_label_;
        const Variable r_label_;
        const Variable lmin_label_;
        const Variable rmax_label_;

        Variable l_l;
        Variable r__l;
        Variable l__l;
        Variable r_l;
        Variable ll_l;
        Variable rl__l;
        Variable ll__l;
        Variable rl_l;


        Variable less;
        Variable lessOrEqual;

        GadgetPtr comparisonGadget;

        DISALLOW_COPY_AND_ASSIGN(R1P_Swap_Gadget);


};

CREATE_GADGET_FACTORY_CLASS_8(
    Swap_Gadget,
    Variable, l,
    Variable, r,
    Variable, lmin,
    Variable, rmax,
    Variable, l_label,
    Variable, r_label,
    Variable, lmin_label,
    Variable, rmax_label
)

Swap_GadgetBase::~Swap_GadgetBase() {};


R1P_Swap_Gadget::R1P_Swap_Gadget(
    ProtoboardPtr pb,
    const Variable& l,
    const Variable& r,
    const Variable& lmin,
    const Variable& rmax,
    const Variable& l_label,
    const Variable& r_label,
    const Variable& lmin_label,
    const Variable& rmax_label
) : Gadget(pb), Swap_GadgetBase(pb), R1P_Gadget(pb), 
    l_(l), r_(r), lmin_(lmin), rmax_(rmax), 
    l_label_(l_label), r_label_(r_label), lmin_label_(lmin_label), rmax_label_(rmax_label){}



void R1P_Swap_Gadget::init(){
    comparisonGadget = Comparison_Gadget::create(pb_,32,l_,r_, less, lessOrEqual);
}

void R1P_Swap_Gadget::generateConstraints(){
    comparisonGadget->generateConstraints();
    pb_->addRank1Constraint(
        l_,
        lessOrEqual,
        l_l,
        "l_ * lessOrEqual = l_l"
    );
    pb_->addRank1Constraint(
        r_,
        1 - lessOrEqual,
        r__l,
        "r_ * (1 - lessOrEqual) = r__l"
    );
    pb_->addRank1Constraint(
        l_,
        1 - lessOrEqual,
        l__l,
        "l_ * (1 - lessOrEqual) = l__l"
    );
    pb_->addRank1Constraint(
        r_,
        lessOrEqual,
        r_l,
        "r_label_ * lessOrEqual = r_l"
    );
    pb_->addRank1Constraint(
        l_label_,
        lessOrEqual,
        ll_l,
        "l_label_ * lessOrEqual = ll_l"
    );
    pb_->addRank1Constraint(
        r_label_,
        1 - lessOrEqual,
        rl__l,
        "r_label_ * (1 - lessOrEqual) = rl__l"
    );
    pb_->addRank1Constraint(
        l_label_,
        1 - lessOrEqual,
        ll__l,
        "l_label_ * (1 - lessOrEqual) = ll__l"
    );
    pb_->addRank1Constraint(
        r_label_,
        lessOrEqual,
        rl_l,
        "r_label_ * lessOrEqual = rl_l"
    );

    //data
    pb_->addRank1Constraint(
        l_l + r__l,
        1,
        lmin_,
        "l * lessOrEqual + r * (1-lessOrEqual) = lmin"
    );
    pb_->addRank1Constraint(
        l__l + r_l,
        1,
        rmax_,
        "l * (1 - lessOrEqual) + r * lessOrEqual = rmax"
    );
    //label
    
    pb_->addRank1Constraint(
        ll_l + rl__l,
        1,
        lmin_label_, 
        "l_label_ * lessOrEqual + r_label_ * (1 - lessOrEqual) = lmin_label_"
    );
    pb_->addRank1Constraint(
        ll__l + rl_l,
        1,
        rmax_label_,
        "l_label_ * (1 - lessOrEqual) + r_label_ * lessOrEqual = rmax_label_"
    );
    // pb_->addRank1Constraint(l_,1,lmin_,"1");
    // pb_->addRank1Constraint(r_,1,rmax_,"2");
    // pb_->addRank1Constraint(l_label_,1,lmin_label_,"3");
    // pb_->addRank1Constraint(r_label_,1,rmax_label_,"4");
    


}

void R1P_Swap_Gadget::generateWitness(){
    comparisonGadget->generateWitness();
    val(l_l) = val(l_) * val(lessOrEqual);
    val(r__l) = val(r_) * (1 - val(lessOrEqual));
    val(l__l) = val(l_) * (1 - val(lessOrEqual));
    val(r_l) = val(r_) * val(lessOrEqual);
    val(ll_l) = val(l_label_) * val(lessOrEqual);
    val(rl__l) = val(r_label_) * (1 - val(lessOrEqual));
    val(ll__l) = val(l_label_) * (1 - val(lessOrEqual));
    val(rl_l) = val(r_label_) * val(lessOrEqual);

    val(lmin_) = val(l_l) + val(r__l);
    val(rmax_) = val(l__l) + val(r_l);
    val(lmin_label_) = val(ll_l) + val(rl__l);
    val(rmax_label_) = val(ll__l) + val(rl_l); 
    // val(lmin_) = val(l_);
    // val(rmax_) = val(r_);
    // val(l_label_) = val(lmin_label_);
    // val(r_label_) = val(rmax_label_);
}


