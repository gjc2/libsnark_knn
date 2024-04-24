#include "mod_circuit.h"
#include <vector>
#include <string>

using namespace gadgetlib2;

CREATE_GADGET_BASE_CLASS(Knn_GadgetBase);

class R1P_Knn_Gadget: public Knn_GadgetBase, public R1P_Gadget{
    private:
        void generateConstraints();
        void generateWitness();

        friend class Knn_Gadget;
    
    public:
        R1P_Knn_Gadget(
            ProtoboardPtr pb,
            const std::vector<VariableArray>& dataset,
            const VariableArray& label,
            const VariableArray& query,
            const Variable& output,
            const Variable& k
        );

        void init();
        //input
        const std::vector<VariableArray> dataset_;
        const VariableArray label_;
        const VariableArray query_;
        const Variable k_;
        const Variable output_;
        

        //inner variable
        
        

        VariableArray sortedLabel_ = VariableArray(LENGTH, "sortedLabel");
        Variable sumk;

        VariableArray innerProduction = VariableArray(LENGTH, "dataset*query");
        VariableArray sortedInnerProduction = VariableArray(LENGTH, "sorted dataset*query");

        Variable rd;

        
        GadgetPtr innerProductGadget[LENGTH];
        GadgetPtr andKGadget;
        GadgetPtr sortGadget;
        GadgetPtr modGadget;

        std::vector<VariableArray>sub;

        DISALLOW_COPY_AND_ASSIGN(R1P_Knn_Gadget);
};

CREATE_GADGET_FACTORY_CLASS_5(
    Knn_Gadget,
    std::vector<VariableArray>, dataset,
    VariableArray, label,
    VariableArray, query,
    Variable, output,
    Variable, k
)

Knn_GadgetBase::~Knn_GadgetBase(){}

R1P_Knn_Gadget::R1P_Knn_Gadget(
    ProtoboardPtr pb,
    const std::vector<VariableArray>& dataset,
    const VariableArray& label,
    const VariableArray& query,
    const Variable& output,
    const Variable& k
): Gadget(pb), Knn_GadgetBase(pb), R1P_Gadget(pb), 
    dataset_(dataset), label_(label), query_(query), output_(output), k_(k){}

void R1P_Knn_Gadget::init(){
    for(int i=0;i<LENGTH;i++){
        const VariableArray temp = VariableArray(DATA_LENGTH, "sub_"+std::to_string(i));
        sub.push_back(temp);
    }
    
    for(int i=0;i<LENGTH;i++){
        
        innerProductGadget[i] = InnerProduct_Gadget::create(
            pb_,
            sub[i],
            sub[i],
            innerProduction[i]
        );
    }
    
    sortGadget = Sort_Gadget::create(
        pb_,
        innerProduction,
        sortedInnerProduction,
        label_,
        sortedLabel_
    );    
    
    andKGadget = andK_Gadget::create(
        pb_,
        sortedLabel_,
        k_,
        sumk
    );
    
    modGadget = Mod_Gadget::create(
        pb_,
        sumk,
        k_,
        output_,
        rd
    );
}


void R1P_Knn_Gadget::generateConstraints(){
    
    for(int i=0;i<LENGTH;i++){
        for(int j=0;j<DATA_LENGTH;j++){
            pb_->addRank1Constraint(
            dataset_[i][j] - query_[j],
            1,
            sub[i][j],
            "data-query = sub"
            );
        }
    }
    
    for(int i=0;i<LENGTH;i++){
        innerProductGadget[i]->generateConstraints();
    }
    sortGadget->generateConstraints();
    andKGadget->generateConstraints();
    modGadget->generateConstraints();
    
}

void R1P_Knn_Gadget::generateWitness(){
    for(int i=0;i<LENGTH;i++){
        for(int j=0;j<DATA_LENGTH;j++){
            val(sub[i][j]) = val(dataset_[i][j]) - val(query_[j]);
        }
    }
    for(int i=0;i<LENGTH;i++){
        innerProductGadget[i]->generateWitness();
    }
    sortGadget->generateWitness();
    andKGadget->generateWitness();
    modGadget->generateWitness();

    EXPECT_EQ(val(dataset_[0][0]), 2);
    EXPECT_EQ(val(dataset_[0][1]), 3);
    EXPECT_EQ(val(dataset_[0][2]), 1);
    EXPECT_EQ(val(dataset_[0][3]), 4);

    EXPECT_EQ(val(dataset_[1][0]), 3);
    EXPECT_EQ(val(dataset_[1][1]), 2);
    EXPECT_EQ(val(dataset_[1][2]), 5);
    EXPECT_EQ(val(dataset_[1][3]), 6);

    EXPECT_EQ(val(dataset_[2][0]), 8);
    EXPECT_EQ(val(dataset_[2][1]), 7);
    EXPECT_EQ(val(dataset_[2][2]), 6);
    EXPECT_EQ(val(dataset_[2][3]), 5);

    EXPECT_EQ(val(dataset_[3][0]), 2);
    EXPECT_EQ(val(dataset_[3][1]), 7);
    EXPECT_EQ(val(dataset_[3][2]), 7);
    EXPECT_EQ(val(dataset_[3][3]), 4);

    EXPECT_EQ(val(dataset_[4][0]), 1);
    EXPECT_EQ(val(dataset_[4][1]), 9);
    EXPECT_EQ(val(dataset_[4][2]), 9);
    EXPECT_EQ(val(dataset_[4][3]), 3);

    EXPECT_EQ(val(label_[0]), 1);
    EXPECT_EQ(val(label_[1]), 2);
    EXPECT_EQ(val(label_[2]), 3);
    EXPECT_EQ(val(label_[3]), 4);
    EXPECT_EQ(val(label_[4]), 5);

    EXPECT_EQ(val(innerProduction[0]), 29);
    EXPECT_EQ(val(innerProduction[1]), 11);
    EXPECT_EQ(val(innerProduction[2]), 45);
    EXPECT_EQ(val(innerProduction[3]), 17);
    EXPECT_EQ(val(innerProduction[4]), 49);

    EXPECT_EQ(val(sortedLabel_[0]), 2);
    EXPECT_EQ(val(sortedLabel_[1]), 4);
    EXPECT_EQ(val(sortedLabel_[2]), 1);
    EXPECT_EQ(val(sortedLabel_[3]), 3);
    EXPECT_EQ(val(sortedLabel_[4]), 5);
    EXPECT_EQ(val(sumk), 7);
    EXPECT_EQ(val(rd),1);
    EXPECT_EQ(val(output_),2);
}







