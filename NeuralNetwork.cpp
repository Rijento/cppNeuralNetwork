#include "NeuralNetwork.hpp"
#include "Neuron.hpp"
#include "Layer.hpp"
#include "Synapse.hpp"
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/writer.h>
#include <fstream>
#include <regex>
#include <random>


// Helper Functions

float randWeight() {
    return 1.0 - static_cast<float>(rand())/static_cast<float>(RAND_MAX/2.0);
}

// Class funcitons

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
    for (int i = 0; i < outputs; i++) { // Create Output Neurons
        std::string id = "o/" + std::to_string(i);
        Neuron* outputNeuron = new Neuron(id, outputLayer);
        outputLayer->addNeuron(outputNeuron);
    }
    // createInitialConnections(); // Connect neurons
}

// For best results, seed the rand function first
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
                Synapse* connection = new Synapse((*inputIterator).second, (*it).second, randWeight());
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
                    Synapse* connection = new Synapse((*firstIt).second, (*secondIt).second, randWeight());
                    (*firstIt).second->addSynapse(connection);
                }
            }
        }

        // connect last hidden layer to output layer
        for (NeuronIterator lastIterator = lastHiddenLayer->getNeurons()->begin(); lastIterator != lastHiddenLayer->getNeurons()->end(); ++lastIterator) {
            for (NeuronIterator outputIterator = outputLayer->getNeurons()->begin(); outputIterator != outputEnd; ++outputIterator) {
                // Assume that there are no connections, because there shouldn't be
                Synapse* connection = new Synapse((*lastIterator).second, (*outputIterator).second, randWeight());
                (*lastIterator).second->addSynapse(connection); // All weights are 1.0
            }
        }
    } else {
        for (NeuronIterator inputIterator = inputLayer->getNeurons()->begin(); inputIterator != inputEnd; ++inputIterator) {
            for (NeuronIterator outputIterator = outputLayer->getNeurons()->begin(); outputIterator != outputEnd; ++outputIterator) {
                // Assume that there are no connections, because there shouldn't be
                Synapse* connection = new Synapse((*inputIterator).second, (*outputIterator).second, randWeight());
                (*inputIterator).second->addSynapse(connection); // All weights are 1.0
            }
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
    // clone->biasNeuron = biasNeuron->clone(clonedNeurons,clone->inputLayer);

    return clone;
}

void NeuralNetwork::serialize(std::string fileName) {
    rapidjson::Document document;
    document.SetObject();
    std::ofstream fileStream(fileName, std::ios_base::trunc);
    rapidjson::OStreamWrapper osw(fileStream);
    rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

    rapidjson::Value nnet(rapidjson::kObjectType);

    nnet.AddMember("inputs", inputs, allocator);
    nnet.AddMember("outputs", outputs, allocator);
    nnet.AddMember("hiddenCount", hiddenNeuronCount, allocator);
    nnet.AddMember("inputLayer", inputLayer->serialize(allocator), allocator);

    rapidjson::Value hlays(rapidjson::kArrayType);

    for(RLayerIterator it = hiddenLayers.rbegin(); it != hiddenLayers.rend(); ++it) {
        hlays.PushBack((*it)->serialize(allocator), allocator);
    }

    nnet.AddMember("hiddenLayers", hlays, allocator);
    nnet.AddMember("outputLayer", outputLayer->serialize(allocator),allocator);

    document.AddMember("NeuralNet", nnet, allocator);
    document.Accept(writer);
}
void NeuralNetwork::deserialize(std::string fileName) { // loads the network from a serialized string
    rapidjson::Document document;
    document.SetObject();
    std::ifstream fileStream(fileName);
    rapidjson::IStreamWrapper isw(fileStream);
    document.ParseStream<rapidjson::kParseFullPrecisionFlag>(isw);

    std::unordered_map<std::string, Neuron*> deserializedNeurons;

    // Clear starting layers
    delete inputLayer;
    delete outputLayer;

    inputLayer = new Layer(0);
    outputLayer = new Layer(-1);
    
    inputs = document["NeuralNet"]["inputs"].GetInt();
    outputs = document["NeuralNet"]["outputs"].GetInt();
    hiddenNeuronCount = document["NeuralNet"]["hiddenCount"].GetInt();
    rapidjson::Value ilayVal(rapidjson::kObjectType);
    ilayVal = document["NeuralNet"]["inputLayer"].GetObject();
    rapidjson::Value hlaysVal(rapidjson::kArrayType);
    hlaysVal = document["NeuralNet"]["hiddenLayers"].GetArray();
    rapidjson::Value olayVal(rapidjson::kObjectType);
    olayVal = document["NeuralNet"]["outputLayer"].GetObject();
    
    // Deserialize output layer
    outputLayer->deserialize(olayVal, deserializedNeurons);

    // Desearialize hidden layers
    hiddenLayers.clear();

    for(rapidjson::SizeType i = 0; i < hlaysVal.Size(); i++) {
        Layer* layer = new Layer();
        // std::string layerString = *hiddenLayerSpl_it++;
        layer->deserialize(hlaysVal[i].GetObject(), deserializedNeurons);
        hiddenLayers.push_front(layer);
    }

    // Deserialize Input Layer
    inputLayer->deserialize(ilayVal, deserializedNeurons);
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

Layer* NeuralNetwork::getInputLayer() {
    return inputLayer;
}

std::list<Layer*> NeuralNetwork::getHiddenLayers() {
    return hiddenLayers;
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
    // biasNeuron->activate(1.0);
    
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
    for(Layer* layer : hiddenLayers) {
        delete layer;
    }
    // delete biasNeuron;

    // NEED TO REDO THIS TO DELETE ALL HIDDEN LAYERS
    // NEED TO DO ALL RECURSIVE SHIT TO DELETE EVERY POINTER INDIVIDUALLY
}