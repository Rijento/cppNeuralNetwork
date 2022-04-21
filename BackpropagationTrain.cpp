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
    
    if(synapse->getTo()->getDXPartial() != NAN) {
        dxcost *= synapse->getTo()->getDXPartial();
    } else {
        if (synapse->getTo()->getSynapses()->size() > 0) {
            Neuron* curr = synapse->getTo();
            float tempsum = 0.0;
            for (SynapseIterator it = curr->getSynapses()->begin(); it != curr->getSynapses()->end(); ++it) {
                tempsum += backpropagate(*it, actual, expected);
            }
            dxcost *= tempsum;
        } else {
            int onum =  std::stoi(synapse->getTo()->getId().substr(2));
            dxcost *= (2.0/(float)(network->outputs)) * dxMSE(actual[onum], expected[onum]);
            bool testval = true;
        }
    }
    float oldWeight = synapse->getWeight();
    float newWeight = oldWeight - alpha*synapse->getFrom()->getOutputLevel()*dxcost;

    // if (synapse->getFrom()->getId().compare("i/0") == 0)

    synapse->setWeight(isnan(newWeight)?oldWeight:newWeight);
    synapse->getFrom()->incrementDXPartial(dxcost*synapse->getWeight());
    return(dxcost*synapse->getWeight());
    
}


void BackpropagationTrain::train(std::vector<std::vector<float>> trainData, std::vector<std::vector<float>> trainLabels) {
    std::vector<float> actual;
    float trainAccuracy = 0.0;
    for(int i = 0; i < trainData.size(); i++) {
        actual = network->feedForward(trainData[i]);
        for (NeuronIterator it = network->inputLayer->getNeurons()->begin(); it != network->inputLayer->getNeurons()->end(); ++it) {
            for(SynapseIterator sit = it->second->getSynapses()->begin(); sit != it->second->getSynapses()->end(); ++sit) {
                backpropagate(*sit, actual, trainLabels[i]);
            }
        }
    }
}

float BackpropagationTrain::test(std::vector<std::vector<float>> testData, std::vector<std::vector<float>> testLabels) {
    std::vector<float> actual;
    std::vector<float> expected;
    float accuracy = 0.0;
    for(int i = 0; i < testData.size(); ++i) {
        float itemAccuracy = 0.0;
        actual = network->feedForward(testData[i]);
        expected = testLabels[i];
        for(int j = 0; j < actual.size(); ++j) {
            itemAccuracy += MSE(actual[j], expected[j]);
        }
        accuracy += itemAccuracy > 0.5 ? 0 : 1;
    }
    return accuracy/testData.size();
}