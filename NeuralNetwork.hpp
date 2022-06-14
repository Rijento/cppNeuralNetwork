#ifndef __NEURALNETWORK_H__
#define __NEURALNETWORK_H__

#include <vector>
#include <list>
#include <string>

#define LayerIterator std::list<Layer*>::iterator
#define RLayerIterator std::list<Layer*>::reverse_iterator

class Synapse;
class Neuron;
class Layer;

class NeuralNetwork {
// Needs to be firends with all of the training algorithms
friend class BackpropagationTrain;
protected:
    Layer* inputLayer;
    Layer* outputLayer;
    std::list<Layer*> hiddenLayers;
    // Neuron* biasNeuron;
    Layer* getRandomLayer();
private:
    int hiddenNeuronCount;
    int inputs;
    int outputs;
public:
    NeuralNetwork();
    NeuralNetwork(int inputsIn, int outputsIn);
    NeuralNetwork*clone(); // creates a copy of the network
    void serialize(std::string fileName); // serializes the network to a JSON file
    void deserialize(std::string fileName); // loads the network from a serialized json file
    Layer* getInputLayer();
    std::list<Layer*>* getHiddenLayers();
    int getHiddenNeuronCount();
    void insertLayer(int index);
    void addNewNode(int depth); // adds a new node to the hidden layer
    std::vector<float> feedForward(std::vector<float> inputData);
    void randomizeWeights();
    void createInitialConnections();
    ~NeuralNetwork();
};
#endif