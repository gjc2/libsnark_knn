#include "swap_circuit.h"
#include <depends/gtest/googletest/include/gtest/gtest.h>
#define DATA_LENGTH 4 
#define LENGTH 5
using namespace gadgetlib2;

CREATE_GADGET_BASE_CLASS(Sort_GadgetBase);

class R1P_Sort_Gadget : public Sort_GadgetBase, public R1P_Gadget{
    public:
        void generateConstraints();
        void generateWitness();

        friend class Sort_Gadget;
    private:
        R1P_Sort_Gadget(
            ProtoboardPtr pb,
            const VariableArray& inputs,
            const VariableArray& outputs,
            const VariableArray& labels,
            const VariableArray& sortedLabels
        );

        void init();

        const VariableArray inputs_;
        const VariableArray outputs_;
        const VariableArray labels_;
        const VariableArray sortedLabels_;

        VariableArray mid[(LENGTH-1)*(LENGTH-1)];

        VariableArray label_mid[(LENGTH-1)*(LENGTH-1)];

        GadgetPtr comparisonGadget[(LENGTH-1)*(LENGTH-1)]; 

        DISALLOW_COPY_AND_ASSIGN(R1P_Sort_Gadget);

};

CREATE_GADGET_FACTORY_CLASS_4(
    Sort_Gadget,
    VariableArray, inputs,
    VariableArray, outputs,
    VariableArray, labels,
    VariableArray, sortedLabels
)


Sort_GadgetBase::~Sort_GadgetBase(){}


R1P_Sort_Gadget::R1P_Sort_Gadget(
    ProtoboardPtr pb,
    const VariableArray& inputs,
    const VariableArray& outputs,
    const VariableArray& labels,
    const VariableArray& sortedLabels
): Gadget(pb), Sort_GadgetBase(pb), R1P_Gadget(pb), 
    inputs_(inputs), outputs_(outputs),
    labels_(labels), sortedLabels_(sortedLabels){}


void R1P_Sort_Gadget::init(){
    //bubble sort
    for(int i=0;i<(LENGTH-1)*(LENGTH-1);i++){
        mid[i] = VariableArray(LENGTH, "mid_"+std::to_string(i));
    }
    for(int i=0;i<(LENGTH-1)*(LENGTH-1);i++){
        label_mid[i] = VariableArray(LENGTH, "label_mid_"+std::to_string(i));
    }
    
    for(int i=0;i<LENGTH-1;i++){
        for(int j=0;j<LENGTH-1;j++){
            
            if(i==0&&j==0){
                comparisonGadget[i*(LENGTH-1)+j] = Swap_Gadget::create(
                    pb_,
                    inputs_[j],
                    inputs_[j+1],
                    mid[i*(LENGTH-1)+j][j],
                    mid[i*(LENGTH-1)+j][j+1],
                    labels_[j],
                    labels_[j+1],
                    label_mid[i*(LENGTH-1)+j][j],
                    label_mid[i*(LENGTH-1)+j][j+1]
                ); 
            }else{
                comparisonGadget[i*(LENGTH-1)+j] = Swap_Gadget::create(
                    pb_,
                    mid[i*(LENGTH-1)+j-1][j],
                    mid[i*(LENGTH-1)+j-1][j+1],
                    mid[i*(LENGTH-1)+j][j],
                    mid[i*(LENGTH-1)+j][j+1],
                    label_mid[i*(LENGTH-1)+j-1][j],
                    label_mid[i*(LENGTH-1)+j-1][j+1],
                    label_mid[i*(LENGTH-1)+j][j],
                    label_mid[i*(LENGTH-1)+j][j+1]
                );
            }
        }
    }
}

void R1P_Sort_Gadget::generateConstraints() {
    for(int i=0;i<LENGTH-1;i++){
        for(int j=0;j<LENGTH-1;j++){
            if(i==0&&j==0){
                for(int k=0;k<LENGTH;k++){
                    if(k==j||k==j+1) {
                        
                    }else{
                        pb_->addRank1Constraint(
                            inputs_[k],
                            1,
                            mid[i*(LENGTH-1)+j][k],
                            "input[k]=>mid[0][k]"
                        );
                        pb_->addRank1Constraint(
                            labels_[k],
                            1,
                            label_mid[i*(LENGTH-1)+j][k],
                            "labels[k]=>label_mid[0][k]"
                        );
                    }
                }
                comparisonGadget[i*(LENGTH-1)+j]->generateConstraints();
            }else{
                for(int k=0;k<LENGTH;k++){
                    if(k==j||k==j+1) {
                        
                    }else{
                        pb_->addRank1Constraint(
                            mid[i*(LENGTH-1)+j-1][k],
                            1,
                            mid[i*(LENGTH-1)+j][k],
                            "mid[i*(LENGTH-1)+j-1][k]=>mid[i*(LENGTH-1)+j][k]"
                        );
                        pb_->addRank1Constraint(
                            label_mid[i*(LENGTH-1)+j-1][k],
                            1,
                            label_mid[i*(LENGTH-1)+j][k],
                            "label_mid[i*(LENGTH-1)+j-1][k]=>label_mid[i*(LENGTH-1)+j][k]"
                        );
                    }
                }
                comparisonGadget[i*(LENGTH-1)+j]->generateConstraints();
            }
        }
    }
    for(int i=0;i<LENGTH;i++){
        pb_ -> addRank1Constraint(
            mid[(LENGTH-1)*(LENGTH-1)-1][i],
            1,
            outputs_[i],
            "mid[(LENGTH-1)*(LENGTH-1)-1][i]=>outputs_[i]"
        );
        pb_ -> addRank1Constraint(
            label_mid[(LENGTH-1)*(LENGTH-1)-1][i],
            1,
            sortedLabels_[i],
            "label_mid[(LENGTH-1)*(LENGTH-1)-1][i]=>sortedLabels_[i]"
        );
    }
}

void R1P_Sort_Gadget::generateWitness() {
    for(int i=0;i<LENGTH-1;i++){
        for(int j=0;j<LENGTH-1;j++){
            if(i==0&&j==0){
                for(int k=0;k<LENGTH;k++){
                    if(k==j||k==j+1) {
                        
                    }else{                    
                        val(mid[i*(LENGTH-1)+j][k]) = val(inputs_[k]);
                        val(label_mid[i*(LENGTH-1)+j][k]) = val(labels_[k]);
                    }
                }
                comparisonGadget[i*(LENGTH-1)+j]->generateWitness();
            }else{
                for(int k=0;k<LENGTH;k++){
                    if(k==j||k==j+1){
                        
                    }else{
                        val(mid[i*(LENGTH-1)+j][k]) = val(mid[i*(LENGTH-1)+j-1][k]);
                        val(label_mid[i*(LENGTH-1)+j][k]) = val(label_mid[i*(LENGTH-1)+j-1][k]);
                    }
                }
                comparisonGadget[i*(LENGTH-1)+j]->generateWitness();
            }
        }
    }

    
    
    for(int i=0;i<LENGTH;i++){
        val(outputs_[i]) = val(mid[(LENGTH-1)*(LENGTH-1)-1][i]);
        val(sortedLabels_[i]) = val(label_mid[(LENGTH-1)*(LENGTH-1)-1][i]);
    }
    

}