#include "NeuralNetwork.hpp"
#include "Neuron.hpp"
#include "Layer.hpp"
#include "Synapse.hpp"

NeuralNetwork::NeuralNetwork(int inputsIn, int outputsIn){
    inputLayer = new Layer(0);
    outputLayer = new Layer(-1);
    hiddenNeuronCount = 0;
    inputs = inputsIn;
    outputs = outputsIn;
    for (int i = 0; i < inputs; i++) { // Create Input Neurons
        std::string id = "i/" + std::to_string(i);
        Neuron* inputNeuron = new Neuron(id, inputLayer);
        inputLayer->addNeuron(inputNeuron);
    }
    //Create Bias Neuron
    biasNeuron = new Neuron("bias", inputLayer);

    for (int i = 0; i < outputs; i++) { // Create Output Neurons
        std::string id = "o/" + std::to_string(i);
        Neuron* outputNeuron = new Neuron(id, outputLayer);
        outputLayer->addNeuron(outputNeuron);
    }
    // createInitialConnections(); // Connect neurons
}

void NeuralNetwork::createInitialConnections() {
    NeuronIterator inputEnd = inputLayer->getNeurons()->end();
    NeuronIterator outputEnd = outputLayer->getNeurons()->end();

    // Add connections for input layer
    for (NeuronIterator inputIterator = inputLayer->getNeurons()->begin(); inputIterator != inputEnd; ++inputIterator) {
        for (NeuronIterator outputIterator = outputLayer->getNeurons()->begin(); outputIterator != outputEnd; ++outputIterator) {
            // Assume that there are no connections, because there shouldn't be
            Synapse* connection = new Synapse((*inputIterator).second, (*outputIterator).second);
            (*inputIterator).second->addSynapse(connection); // All weights are 1.0
        }
    }
    // Add connections for bias neuron
    for (NeuronIterator outputIterator = outputLayer->getNeurons()->begin(); outputIterator != outputEnd; ++outputIterator) {
        // Assume that there are no connections, because there shouldn't be
        Synapse* connection = new Synapse(biasNeuron, (*outputIterator).second);
        biasNeuron->addSynapse(connection); // All weights are 1.0
    }
}

NeuralNetwork* NeuralNetwork::clone() { // creates a copy of the network TODO: THIS
    NeuralNetwork* clone = new NeuralNetwork(inputs, outputs);
    std::unordered_map<std::string, Neuron*> clonedNeurons;

    clone->outputLayer = outputLayer->clone(clonedNeurons);
    std::list<Layer*> clonedHiddenLayers = {};
    // Need to reverse through to ensure that neurons are included in the clonedNeurons map before the synapses connected to them try to clone
    for (RLayerIterator it = hiddenLayers.rbegin(); it != hiddenLayers.rend(); ++it) {
        clonedHiddenLayers.push_front((*it)->clone(clonedNeurons)); //Push front becasue iterating through in reverse
    }

    clone->hiddenLayers = clonedHiddenLayers;
    
    clone->inputLayer = inputLayer->clone(clonedNeurons);
    clone->biasNeuron = biasNeuron->clone(clonedNeurons,clone->inputLayer);

    return clone;
}

std::string NeuralNetwork::serialize() {
    std::string serialized = "NN:(" + std::to_string(inputs) + "," + std::to_string(outputs) + "," + std::to_string(hiddenNeuronCount)
        + ",ilay:(";
    serialized += inputLayer->serialize();
    serialized += "),bnur:(" + biasNeuron->serialize();
    serialized += "),hlays:(";

    for(LayerIterator it = hiddenLayers.begin(); it != hiddenLayers.end(); ++it) {
        serialized += (*it)->serialize();
        serialized += ",";
    }

    serialized += "),olay:(" + outputLayer->serialize();
    serialized += "))";

    return serialized;

}
void NeuralNetwork::deserialize(std::string dataIn) { // loads the network from a serialized string

}

Layer* NeuralNetwork::getRandomLayer() {

}


void NeuralNetwork::insertLayer(int index) {
    Layer* layer = new Layer(index);
    LayerIterator it = hiddenLayers.begin();
    std::advance(it, index);
    hiddenLayers.insert(it, layer);

    for(;it != hiddenLayers.end(); it++) {
        ++(*it);
    }

}
std::vector<float> NeuralNetwork::feedForward(std::vector<float> inputData) {
    for(int i = 0; i < inputData.size(); i++) { // These need to be activated in order along side the input data
        std::string id = "i/" + std::to_string(i);
        inputLayer->getNeurons()->at(id)->activate(inputData[i]);
    }
    biasNeuron->activate(1.0);
    
    for(LayerIterator it = hiddenLayers.begin(); it != hiddenLayers.end(); ++it) { // should activate them in order
        for(NeuronIterator nit = (*it)->getNeurons()->begin(); nit != (*it)->getNeurons()->end(); ++nit) { // order doesn't matter so long as they are all activated
            nit->second->activate();
        }
    }

    std::vector<float> outputData;

    for (int i = 0; i < outputs; i++) { // Activate output layer and record outputs
        std::string id = "o/" + std::to_string(i);
        outputLayer->getNeurons()->at(id)->activate();
        outputData[i] = outputLayer->getNeurons()->at(id)->getOutputLevel();
    }
    
    return outputData;
}

NeuralNetwork::~NeuralNetwork() {
    delete inputLayer;
    delete outputLayer;
    delete biasNeuron;
}