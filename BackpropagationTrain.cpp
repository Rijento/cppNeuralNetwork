#include "BackpropagationTrain.hpp"
#include "NeuralNetwork.hpp"
#include "Layer.hpp"
#include "Neuron.hpp"
#include "Synapse.hpp"
#include <math.h>


// Helper Functions
float dxsigmoid(float val) {
    return exp(val)/pow(exp(val)+1,2);
}
float dxMSE(float actual, float expected) {
    return 2.0*(actual - expected);
}

// Class Functions

BackpropagationTrain::BackpropagationTrain(NeuralNetwork* networkIn, float alphaIn) {
    network = networkIn;
    alpha = alphaIn;
}

float BackpropagationTrain::MSE(float actual, float expected) {
    return pow(expected - actual, 2);
}

float BackpropagationTrain::backpropagate(Synapse* synapse, std::vector<float>& actual, std::vector<float>& expected) {
    float dxcost = dxsigmoid(synapse->getTo()->getArchivedLevel());
    if (synapse->getTo()->getSynapses()->size() > 0) {
        Neuron* curr = synapse->getTo();
        float tempsum = 0.0;
        for (SynapseIterator it = curr->getSynapses()->begin(); it != curr->getSynapses()->end(); ++it) {
            tempsum += backpropagate(*it, actual, expected);
        }
        dxcost *= tempsum;
    } else {
        int onum =  std::stoi(synapse->getTo()->getId().substr(1));
        dxcost *= (2.0/(float)(network->outputs)) * dxMSE(actual[onum], expected[onum]);
    }
    float oldWeight = synapse->getWeight();
    synapse->setWeight(oldWeight - alpha*synapse->getFrom()->getArchivedLevel()*dxcost);
    return(dxcost*synapse->getWeight());
}


void BackpropagationTrain::train(std::vector<std::vector<float>> trainData, std::vector<std::vector<float>> trainLabels) {
    std::vector<float> actual;
    for(int i = 0; i < trainData.size(); i++) {
        actual = network->feedForward(trainData[i]);
        for (NeuronIterator it = network->inputLayer->getNeurons()->begin(); it != network->inputLayer->getNeurons()->end(); ++it) {
            for(SynapseIterator sit = it->second->getSynapses()->begin(); sit != it->second->getSynapses()->end(); ++sit) {
                backpropagate(*sit, actual, trainLabels[i]);
            }
        }
    }

}