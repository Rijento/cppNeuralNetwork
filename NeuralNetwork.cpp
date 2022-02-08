#include "NeuralNetwork.hpp"
#include "Neuron.hpp"
#include "Layer.hpp"
#include "Synapse.hpp"
#include <regex>

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
    if (hiddenLayers.size() > 0) {
        Layer* firstHiddenLayer = *hiddenLayers.begin();
        Layer* lastHiddenLayer = *hiddenLayers.rbegin();
        // Add connections between input layer and first hidden layer
        for (NeuronIterator inputIterator = inputLayer->getNeurons()->begin(); inputIterator != inputEnd; ++inputIterator) { 
            for(NeuronIterator it = firstHiddenLayer->getNeurons()->begin(); it != firstHiddenLayer->getNeurons()->end(); ++it) {
                Synapse* connection = new Synapse((*inputIterator).second, (*it).second);
                (*inputIterator).second->addSynapse(connection);
            }
        }

        // connect hidden layers together
        LayerIterator it = hiddenLayers.begin();
        while (*it != *hiddenLayers.rbegin()) {
            LayerIterator curr = it++;
            std::unordered_map<std::string, Neuron*>* firstNeurons = (*curr)->getNeurons();
            std::unordered_map<std::string, Neuron*>* secondNeurons = (*it)->getNeurons();
            for (NeuronIterator firstIt = firstNeurons->begin(); firstIt != firstNeurons->end(); ++firstIt) {
                for (NeuronIterator secondIt = secondNeurons->begin(); secondIt != secondNeurons->end(); ++secondIt) {
                    Synapse* connection = new Synapse((*firstIt).second, (*secondIt).second);
                    (*firstIt).second->addSynapse(connection);
                }
            }
        }

        // connect last hidden layer to output layer
        for (NeuronIterator lastIterator = lastHiddenLayer->getNeurons()->begin(); lastIterator != lastHiddenLayer->getNeurons()->end(); ++lastIterator) {
            for (NeuronIterator outputIterator = outputLayer->getNeurons()->begin(); outputIterator != outputEnd; ++outputIterator) {
                // Assume that there are no connections, because there shouldn't be
                Synapse* connection = new Synapse((*lastIterator).second, (*outputIterator).second);
                (*lastIterator).second->addSynapse(connection); // All weights are 1.0
            }
        }

        // Add connections for bias neuron
        for (NeuronIterator outputIterator = outputLayer->getNeurons()->begin(); outputIterator != outputEnd; ++outputIterator) {
            // Assume that there are no connections, because there shouldn't be
            Synapse* connection = new Synapse(biasNeuron, (*outputIterator).second);
            biasNeuron->addSynapse(connection); // All weights are 1.0
        }
    } else {
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

    // Serialize in reverse order to ensure that everything loads properly when deserializing.
    for(RLayerIterator it = hiddenLayers.rbegin(); it != hiddenLayers.rend(); ++it) {
        serialized += (*it)->serialize();
        serialized += ",";
    }

    serialized += "),olay:(" + outputLayer->serialize();
    serialized += "))";

    return serialized;

}
void NeuralNetwork::deserialize(std::string dataIn) { // loads the network from a serialized string
    std::unordered_map<std::string, Neuron*> deserializedNeurons;
    std::regex_token_iterator<std::string::iterator> rend;

    std::vector<std::string> testing;

    std::regex split ("(\\d*),(\\d*),(\\d*),ilay:\\((.*)\\),bnur:\\((.*)\\),hlays:\\((.*)\\),olay:\\((.*)\\)\\)");
    int submatches[] = {1, 2, 3, 4, 5, 6, 7};
    std::regex_token_iterator<std::string::iterator> spl_it (dataIn.begin(), dataIn.end(), split, submatches);

    std::string inputsString = *spl_it++;
    std::string outputsString = *spl_it++;
    std::string hNursString = *spl_it++;
    std::string ilayString = *spl_it++;
    std::string bnurString = *spl_it++;
    std::string hlaysString = *spl_it++;
    std::string olayString = *spl_it++;

    inputs = std::stoi(inputsString);
    outputs = std::stoi(outputsString);
    hiddenNeuronCount = std::stoi(hNursString);

    // Deserialize output layer
    outputLayer->deserialize(olayString, deserializedNeurons);

    // Desearialize hidden layers
    hiddenLayers.clear();
    std::regex hiddenLayerSplit ("lay:([^l]*\\))*?\\)");
    std::regex_token_iterator<std::string::iterator> hiddenLayerSpl_it (hlaysString.begin(), hlaysString.end(), hiddenLayerSplit);

    while (hiddenLayerSpl_it != rend) {
        Layer* layer = new Layer();
        std::string layerString = *hiddenLayerSpl_it++;
        layer->deserialize(layerString, deserializedNeurons);
        hiddenLayers.push_front(layer);
    }

    // Deserialize Input Layer
    inputLayer->deserialize(ilayString, deserializedNeurons);

    // Deserialize the Bias Neuron
    biasNeuron->deserialize(bnurString, deserializedNeurons, inputLayer);

    bool test = false;
}

Layer* NeuralNetwork::getRandomLayer() {
    int randint = rand() % hiddenLayers.size()+1;

    if (randint == 0) {
        return inputLayer;
    } else {
        LayerIterator it = hiddenLayers.begin();
        std::advance(it, randint-1);
        return *it;
    }

}


void NeuralNetwork::insertLayer(int index) {
    Layer* layer = new Layer(index);
    LayerIterator it = hiddenLayers.begin();
    std::advance(it, index);
    hiddenLayers.insert(it, layer);

    for(;it != hiddenLayers.end(); it++) {
        ++(**it);
    }
}

void NeuralNetwork::addNewNode(int depth) {
    std::string id = "h/"+ std::to_string(hiddenNeuronCount++);
    LayerIterator it = hiddenLayers.begin();
    std::advance(it, depth);
    (*it)->addNeuron(new Neuron(id, *it));
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

    std::vector<float> outputData = *(new std::vector<float>(outputs,0.0));

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