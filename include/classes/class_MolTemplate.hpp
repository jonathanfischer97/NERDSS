/*! \file class_moltemplate.hpp

 * ### Created on 10/18/18 by Matthew Varga
 * ### Purpose
 * ***
 *
 * ### Notes
 * ***
 *
 * ### TODO List
 * ***
 */

#pragma once

#include "classes/class_Coord.hpp"

/*! \defgroup Templates
 * \brief Classes holding information on each Molecule type.
 */

struct Interface {
    /*!  \struct Interface
     * \ingroup Templates
     * \brief Contains information of a MolTemplate's interfaces
     */
    struct Index {
        /*!
         * \brief Just contains the absolute and relative interface indices of the reactions pertinent to a state
         */

        int relative; //!< index relative to each particular reaction, e.g. the vector index of a particular ForwardRxn
        int absolute; //!< absolute index of the reaction a pair of a ForwardRxn and BackRxn will
        //!< have the same relative index (probably), but different absolute indices

        Index() = default;
        Index(int relative, int absolute)
            : relative(relative)
            , absolute(absolute)
        {
        }
    };

    struct State {
        /*! \struct Interface::State
         * \brief Holds information for each state of an Interface
         *
         *   - Reactions are specific to States, not interfaces, as the reactions an interface can participate in can
         * depend on the state (e.g. phosphorylated proteins participarting in different sets of binding events than
         * unphosphorylated proteins).
         */

        std::string ifaceAndStateName {};
        char iden { '\0' }; //!< identity of the state, i.e. the character after the tilde
        int index { -1 };
        std::vector<unsigned> myForwardRxns {}; //!< indices of the reactions in which the state is a reactant (ForwardRxn)
        std::vector<unsigned> myCreateDestructRxns {}; //!< indices of the reactions in which the state is created or destroyed (CreateDestructRxn)
        std::vector<unsigned> rxnPartners {};
        std::vector<std::pair<int, int>> stateChangeRxns {}; //!< indices of state change reactions (forward, back)
        static int totalNumOfStates; //!< total number of states in the system (should be same as totalIfaceNum provided in Parameter file

        State() = default;
        explicit State(int index);
        State(char iden, int index)
            : iden(iden)
            , index(index)
        {
        }
        State(const std::string& ifaceAndStateName, int index);
        State(const std::string& ifaceAndStateName, char iden, int index);
        ~State() = default;

        /*
        Function serialize serializes the State into array of bytes.
        */
        void serialize(unsigned char *arrayRank, int &nArrayRank) {
            // std::cout << "+State serialization nArrayRanks here..." << std::endl;
            serialize_string(ifaceAndStateName, arrayRank, nArrayRank);
            PUSH(iden);
            PUSH(index);
            serialize_primitive_vector<unsigned>(myForwardRxns, arrayRank, nArrayRank);
            serialize_primitive_vector<unsigned>(myCreateDestructRxns, arrayRank, nArrayRank);
            serialize_primitive_vector<unsigned>(rxnPartners, arrayRank, nArrayRank);

            // serialize stateChangeRxns vector of std::pair<int, int>
            *((int *)&(arrayRank[nArrayRank])) = stateChangeRxns.size();
            nArrayRank += sizeof(int);
            for (auto &it : stateChangeRxns) {
                *((int *)&(arrayRank[nArrayRank])) = it.first;
                nArrayRank += sizeof(int);
                *((int *)&(arrayRank[nArrayRank])) = it.second;
                nArrayRank += sizeof(int);
            }

            PUSH(totalNumOfStates);
            // std::cout << "+Total State size in bytes: " << nArrayRank << std::endl;
        }
        void deserialize(unsigned char *arrayRank, int &nArrayRank) {
            deserialize_string(ifaceAndStateName, arrayRank, nArrayRank);
            POP(iden);
            POP(index);
            deserialize_primitive_vector<unsigned>(myForwardRxns, arrayRank, nArrayRank);
            deserialize_primitive_vector<unsigned>(myCreateDestructRxns, arrayRank, nArrayRank);
            deserialize_primitive_vector<unsigned>(rxnPartners, arrayRank, nArrayRank);

            int vector_size = *((int *)(arrayRank + nArrayRank));
            nArrayRank += sizeof(int);
            for (int i = 0; i < vector_size; i++) {
                std::pair<int, int> temp;
                temp.first = *((int *)&(arrayRank[nArrayRank]));
                nArrayRank += sizeof(int);
                temp.second = *((int *)&(arrayRank[nArrayRank]));
                nArrayRank += sizeof(int);
                stateChangeRxns.push_back(temp);
            }

            POP(totalNumOfStates);
        }
    };

    struct RxnState {
    };

    int index { -1 }; //!< relative index of the interface
    Coord iCoord { 0, 0, 0 }; //!< coordinate of this interface
    std::string name; //!< the name of the interface, as provided in the mol file and Parameter file
    std::vector<State> stateList; //!< list of the states an interface can have
    std::vector<int> excludeVolumeBoundList {}; //!< list of mol type index that this molecule interface exclude volume with when it is bound
    std::vector<int> excludeVolumeBoundIfaceList {}; //!< list of interface rel index that this molecule interface exclude volume with when it is bound
    std::vector<int> excludeVolumeBoundReactList {}; //!< list of reaction index that this molecule interface exclude volume with when it is bound
    std::vector<double> excludeRadiusList {}; //!< list of the exclude bindRaius for each interface
    void set_ifaceAndStateNames(); //!< sets the full name for each state (i.e. ifaceName~state). used only in parameter file parsing.

    explicit Interface() = default;
    Interface(std::string name, const Coord& iCoord);
    Interface(std::string name, std::vector<State> states, Coord iCoord);

    /*
    Function serialize serializes the Interface into arrayRank of bytes.
    */
    void serialize(unsigned char *arrayRank, int &nArrayRank) {
        // std::cout << "+Interface serialization nArrayRanks here..." << std::endl;
        PUSH(index);
        iCoord.serialize(arrayRank, nArrayRank);
        serialize_string(name, arrayRank, nArrayRank);
        serialize_abstract_vector<State>(stateList, arrayRank, nArrayRank);
        serialize_primitive_vector<int>(excludeVolumeBoundList, arrayRank, nArrayRank);
        serialize_primitive_vector<int>(excludeVolumeBoundIfaceList, arrayRank, nArrayRank);
        serialize_primitive_vector<int>(excludeVolumeBoundReactList, arrayRank, nArrayRank);
        serialize_primitive_vector<double>(excludeRadiusList, arrayRank, nArrayRank);
        // std::cout << "+Total Interface size in bytes: " << nArrayRank <<
        // std::endl;
    }
    void deserialize(unsigned char *arrayRank, int &nArrayRank) {
        POP(index);
        iCoord.deserialize(arrayRank, nArrayRank);
        deserialize_string(name, arrayRank, nArrayRank);
        deserialize_abstract_vector<State>(stateList, arrayRank, nArrayRank);
        deserialize_primitive_vector<int>(excludeVolumeBoundList, arrayRank, nArrayRank);
        deserialize_primitive_vector<int>(excludeVolumeBoundIfaceList, arrayRank, nArrayRank);
        deserialize_primitive_vector<int>(excludeVolumeBoundReactList, arrayRank, nArrayRank);
        deserialize_primitive_vector<double>(excludeRadiusList, arrayRank, nArrayRank);
    }
};

struct ParsedMolNumState {
    /*! \struct ParsedMolNumState
     * \ingroup Parser
     * \brief Holds MolTemplate information temporarily during input file parsing for the starting copy numbers each state
     */

    int totalCopyNumbers { 0 }; //!< total copy numbers for one molTemplate
    std::vector<int> numberEachState {}; //!< copy numbers for each state
    std::vector<std::string> nameEachState {}; //!< iface~state for each state

    ParsedMolNumState() = default;

    /*
    Function serialize serializes the ParsedMolNumState into arrayRank of bytes.
    */
    void serialize(unsigned char *arrayRank, int &nArrayRank) {
        PUSH(totalCopyNumbers);
        serialize_primitive_vector<int>(numberEachState, arrayRank, nArrayRank);
        serialize_vector_strings(nameEachState, arrayRank, nArrayRank);
    }
    void deserialize(unsigned char *arrayRank, int &nArrayRank) {
        POP(totalCopyNumbers);
        deserialize_primitive_vector<int>(numberEachState, arrayRank, nArrayRank);
        deserialize_vector_strings(nameEachState, arrayRank, nArrayRank);
    }
};

struct MolTemplate {
    /*! \struct MolTemplate
     * \ingroup Templates
     * \brief Data structure for holding the molecule types.
     *
     * Each molecule type will have a template object, which contains the properties
     * of a molecule shared by all Molecule class objects of that type
     */
    Coord comCoord { 0, 0, 0 }; //!< Center of mass coordinate
    bool checkOverlap { false }; //!< is overlap checked for during association?
    bool countTransition { false }; //!< is transition counted during whole simulation?
    int transitionMatrixSize {500}; //!< size of the transition matrix
    bool insideCompartment {false}; //!< this mol type is inside the compartment
    bool outsideCompartment {false}; //!< this mol type is outside the compartment
    bool crossesCompartment {false}; //!< this mol type can cross the compartment
    int transmissionRxnIndex {-1};  //this points to reaction index in transmission reaction list  
    std::vector<std::vector<long long int> > transitionMatrix {}; //!< transition matrix to track the changing of the size of cluster
    std::vector<std::vector<double> > lifeTime {}; //!< to store the lifetime of each cluster size, unit: s 
    //    double bindSepFactor { 1.0 }; //!< separation factor for binding within the same complex
    /*HOW IS COPIES DIFFERENT THAN NumEachMolType?*/
    int copies { 0 }; //!< initial copy numbers of this molecule
    int molTypeIndex { 0 }; //!< the index of this MolTemplate (the same as the molTemplateList index of this object)
    double mass { -1.0 }; //!< the mass of the Molecules of this template
    double radius { 0.0001 }; //!< 'radius' of the protein (i.e. the length of the longest COM-iface vector). in nm. MUST BE NONZERO FOR DIFFUSION COF UPDATES.

    Coord D { 0, 0, 0 }; //!< the molecule's xyz translational diffusion constants
    /**< Einstein-Stokes Equation: \f$ D = \frac{k_B T}{6 \pi \eta \mu r}\f$ */
    Coord Dr { 0, 0, 0 }; //!< the molecule's xyz rotational diffusion constants
    std::string molName; //!< the name of the molecule this is the template for
    std::vector<Interface> interfaceList {}; //!< list of all interfaces on this molecule
    std::vector<int> rxnPartners {}; //!< list of MolTemplate indices that this molecule can react with
    std::vector<std::array<int, 2>> bondList {}; //!< bonds between interfaces/COM, optionally given by the user
    std::vector<int> ifacesWithStates {}; //!< list of interface indicies that have states
    ParsedMolNumState startingNumState {}; //!< record the starting numbers of each state
    bool canDestroy { false }; //!< set this true when there is destruction reaction for this mol type
    bool excludeVolumeBound { false }; //!< set this true when we need exclude volume for bound interface of this mol type
    std::vector<int> monomerList {}; //!< list of the molecule index that is a monomer with trajStatus::none, used for the destruction and transmission; only update when canDestroy is true

    // Molecule types, for checking to perform certain actions
    bool isRod { false }; //!< is the molecule a rod (is it strictly one dimensional)
    bool isLipid { false }; //!< is the molecule a lipid?
    bool isPromoter {false}; //!< is the molecule a promoter for transcription initiation
    bool isPoint { false }; //!< is the Molecule a point (i.e., do all its interfaces overlap with the COM)
    bool isImplicitLipid { false }; //!< is the molecule an implicit lipid? to use, must specify implicitLipid=true in boundaries.

    bool bindToSurface { false }; //For use with continuum membrane binding method. 0 (default) means no surface adsorption.
    // functions to display values
    void display() const;
    void display(const std::string& name) const;

    static std::map<const std::string, MolKeyword> molKeywords; //!< keywords for file parsing see MolKeywords
    static std::vector<int> absToRelIface; //!< list of relative iface indices indexed by absolute indices
    /*This element below numMolTypes is called in Complex::Complex() function calls.*/
    static unsigned numMolTypes; //!< number of molecule types in system (== molTemplateList.size())
    static std::vector<int> numEachMolType; //!< array with length numMolTypes which holds the number of each in system

    // functions to find values
    int find_relIndex_from_absIndex(int targStateIndex) const;
    int find_absIndex_from_relIndex(int relIndex, char state) const;
    void set_value(std::string& line, MolKeyword molKeyword);

    MolTemplate() = default;
    MolTemplate(Coord& comCoord, std::vector<Interface>& Interfaces);
    MolTemplate(std::string molName, std::vector<Interface>& interfaceList);

    /*
    Function serialize serializes the MolTemplate into array of bytes.
    */
    void serialize(unsigned char *arrayRank, int &nArrayRank) {
        // std::cout << "+MolTemplate serialization nArrayRanks here..." <<
        // std::endl;
        //  Serialize starting from beginning of arrayRank
        //  increased by the number of bytes already serialized:
        comCoord.serialize(arrayRank, nArrayRank);
        PUSH(checkOverlap);
        PUSH(countTransition);
        PUSH(transitionMatrixSize);
        serialize_primitive_matrix<long long int>(transitionMatrix, arrayRank, nArrayRank);
        serialize_primitive_matrix<double>(lifeTime, arrayRank, nArrayRank);
        //    double bindSepFactor { 1.0 }; //!< separation factor for binding
        //    within the same complex
        // HOW IS COPIES DIFFERENT THAN NumEachMolType?
        PUSH(copies);
        PUSH(molTypeIndex);
        PUSH(mass);
        PUSH(radius);
        D.serialize(arrayRank, nArrayRank);
        Dr.serialize(arrayRank, nArrayRank);
        serialize_string(molName, arrayRank, nArrayRank);
        serialize_abstract_vector<Interface>(interfaceList, arrayRank, nArrayRank);
        serialize_primitive_vector<int>(rxnPartners, arrayRank, nArrayRank);

        serialize_vector_array<int, 2>(bondList, arrayRank, nArrayRank);

        serialize_primitive_vector<int>(ifacesWithStates, arrayRank, nArrayRank);
        startingNumState.serialize(arrayRank, nArrayRank);
        PUSH(canDestroy);
        PUSH(excludeVolumeBound);
        PUSH(isRod);
        PUSH(isLipid);
        PUSH(isPoint);
        PUSH(isImplicitLipid);
        PUSH(bindToSurface);
        // std::cout << "+Total MolTemplate size in bytes: " << nArrayRank <<
        // std::endl;
    }
    void deserialize(unsigned char *arrayRank, int &nArrayRank) {
        comCoord.deserialize(arrayRank, nArrayRank);
        POP(checkOverlap);
        POP(countTransition);
        POP(transitionMatrixSize);
        deserialize_primitive_matrix<long long int>(transitionMatrix, arrayRank, nArrayRank);
        deserialize_primitive_matrix<double>(lifeTime, arrayRank, nArrayRank);
        //    double bindSepFactor { 1.0 }; //!< separation factor for binding
        //    within the same complex
        // HOW IS COPIES DIFFERENT THAN NumEachMolType?
        POP(copies);
        POP(molTypeIndex);
        POP(mass);
        POP(radius);
        D.deserialize(arrayRank, nArrayRank);
        Dr.deserialize(arrayRank, nArrayRank);
        deserialize_string(molName, arrayRank, nArrayRank);
        deserialize_abstract_vector<Interface>(interfaceList, arrayRank, nArrayRank);
        deserialize_primitive_vector<int>(rxnPartners, arrayRank, nArrayRank);

        deserialize_vector_array<int, 2>(bondList, arrayRank, nArrayRank);

        deserialize_primitive_vector<int>(ifacesWithStates, arrayRank, nArrayRank);
        startingNumState.deserialize(arrayRank, nArrayRank);
        POP(canDestroy);
        POP(excludeVolumeBound);
        POP(isRod);
        POP(isLipid);
        POP(isPoint);
        POP(isImplicitLipid);
        POP(bindToSurface);
    }
};
