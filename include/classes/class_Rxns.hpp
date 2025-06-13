/*! \file class_rxns

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

#include "classes/class_Molecule_Complex.hpp"

#include <cmath>
#include <limits>
#include <vector>

/*! \defgroup Reactions
 * \brief Classes and functions specific to reaction events.
 */

extern unsigned long totMatches;

/*! \enum ReactionType
 * \brief Enumeration of reaction types
 */
enum struct ReactionType : int {
    none = -1, //!< default for initialization
    uniMolStateChange = 0, //!< unimolecular state change reaction (X <-> X*)
    bimolecular = 1, //!< bimolecular reaction. Can include either state or interaction changes, but not both at once.
    biMolStateChange = 2,
    zerothOrderCreation = 3, //!< creation reaction from concentration (0 -> X).
    destruction = 4, //!< destruction reactions. Destroys entire complex, not just interface.
    uniMolCreation = 5, //!< creation reaction from Molecule (X -> X + Y).
    bindToSurface = 6,
    transmission = 7, //!< transmission reaction cross the compartment
};
std::ostream& operator<<(std::ostream& os, const ReactionType& rxnType);

struct RxnIface {
    /*! \struct RxnIface
     * \ingroup SimulClasses
     * \ingroup Reactions
     * \brief Holds information on one reactant or product for a reaction (RxnBase)
     */

    std::string ifaceName; //!< name of this reactant/product
    int molTypeIndex { 0 }; //!< index of the molecule the iface is a member of
    int absIfaceIndex { 0 }; //!< absolute index of this reactant/product
    int relIfaceIndex { 0 }; //!< relative index of this reactant/product in MolTemplate.interfaceList
    char requiresState { '\0' }; //!< the state required for this reaction to occur (Interface::State.iden)
    bool requiresInteraction { false }; //!< does the interface need to be bound to something

    bool operator==(const Molecule::Iface&) const;
    friend std::ostream& operator<<(std::ostream& os, const RxnIface& oneIface);

    RxnIface() = default;
    explicit RxnIface(std::string ifaceName, int molTypeIndex, int absIfaceIndex, int relIfaceIndex, char requiresState, bool requiresInteraction);

    /*
    Function serialize serializes the RxnIface into arrayRank of bytes.
    */
    void serialize(unsigned char* arrayRank, int& nArrayRank) {
        serialize_string(ifaceName, arrayRank, nArrayRank);
        PUSH(molTypeIndex);
        PUSH(absIfaceIndex);
        PUSH(relIfaceIndex);
        PUSH(requiresState);
        PUSH(requiresInteraction);
    }
    /*
    Function deserialize deserializes the RxnIface from given arrayRank of bytes.
    */
    void deserialize(unsigned char* arrayRank, int& nArrayRank) {
        deserialize_string(ifaceName, arrayRank, nArrayRank);
        POP(molTypeIndex);
        POP(absIfaceIndex);
        POP(relIfaceIndex);
        POP(requiresState);
        POP(requiresInteraction);
    }
};

enum class ReactionType; // forward declaration
struct RxnBase {
    /*! \ingroup Reactions
     * \brief Abstract class holding information applicable to all reactions (ForwardRxn, BackRxn, and
     * CreateDestructRxn)
     */

public:
    struct RateState {
        /*! \struct RxnState
         * \brief Container for the different rates a reaction might have, dependent on interface states, bound status,
         *  etc.
         */
        double rate { 0.0 }; //!< rate for this reaction state (if no conditional reactions, this is the only rate)
        double prob { 0.0 }; //!< reaction probability, only used for zeroth and first order reactions (constant)
        std::vector<std::vector<RxnIface>> otherIfaceLists {}; //!< list of ancillary interfaces which must be present for the reaction to occur

        RateState() = default;
        RateState(double rate, const std::vector<std::vector<RxnIface>>& otherIfaceList)
            : rate(rate)
            , otherIfaceLists(otherIfaceList)
        {
        }

        /*
        Function serialize serializes the RateState into arrayRank of bytes.
        */
        void serialize(unsigned char* arrayRank, int& nArrayRank) {
            PUSH(rate);
            PUSH(prob);
            // serialize otherIfaceLists matrix of RxnIface
            serialize_abstract_matrix<RxnIface>(otherIfaceLists, arrayRank, nArrayRank);
        }
        /*
        Function deserialize deserializes the RateState from given arrayRank of bytes.
        */
        void deserialize(unsigned char* arrayRank, int& nArrayRank) {
            POP(rate);
            POP(prob);
            deserialize_abstract_matrix<RxnIface>(otherIfaceLists, arrayRank, nArrayRank);
        }
    };

    struct CoupledRxn {
        int absRxnIndex { -1 }; //!< absolute index of coupled reaction
        int relRxnIndex { -1 }; //!< relative index of the coupled reaction
        ReactionType rxnType { ReactionType::none }; //!< type of the coupled reaction
        std::string label { "none" }; //!< label of the coupled reaction
        double probCoupled { 1 }; //!< perform coupled reaction with this prob.
        CoupledRxn() = default;
        CoupledRxn(int _absRxnIndex)
            : absRxnIndex(_absRxnIndex)
        {
        }
        CoupledRxn(std::string _label)
            : label(_label)
        {
        }

        /*
        Function serialize serializes the RxnIface into arrayRank of bytes.
        */
        void serialize(unsigned char* arrayRank, int& nArrayRank) {
            PUSH(absRxnIndex);
            PUSH(relRxnIndex);
            PUSH(rxnType);
            serialize_string(label, arrayRank, nArrayRank);
            PUSH(probCoupled);
        }
        /*
        Function deserialize deserializes the RxnIface from given arrayRank of bytes.
        */
        void deserialize(unsigned char* arrayRank, int& nArrayRank) {
            POP(absRxnIndex);
            POP(relRxnIndex);
            POP(rxnType);
            deserialize_string(label, arrayRank, nArrayRank);
            POP(probCoupled);
        }
    };

    // Temporary Observable Feature
    bool isObserved { false }; //!< is the product observed?
    std::string observeLabel {}; //!< label under which the product is written to the observables file
    // below should be per each reaction rate.
    double loopCoopFactor { 1.0 }; //!< multiple the rate by this factor, used only when closing loops
    double bindRadSameCom { 1.1 }; //!< distance between two reactants to force reaction within the same complex
    /**< reactant and product arrays*/
    bool isSymmetric { false }; //!< are both reactants of the reaction identical (interfaces and states)
    bool isOnMem { false }; //!< does the reaction occur only on the membrane
    bool hasStateChange { false }; //!< does this reaction include a state change
    ReactionType rxnType {}; //!< what is the type of this reaction (see ReactionType)
    int absRxnIndex { 0 }; //!< absolute index of the reaction
    int relRxnIndex { 0 }; //!< absolute index of the reaction
    bool isCoupled { false };
    double length3Dto2D { -1 }; //!< in nm, length scale to convert 3D rate to 2D rate, by default will be set to 2* bindrad if not read in from file.
    double area3Dto1D { -1 }; //!< in nm^2, area scale to convert 3D rate to 1D rate, by default will be set to 4*pi*bindrad^2
    std::string rxnLabel { "nonspecified" }; //!< label of the reaction, used for coupled; default value is nonspecified
    std::string coupledRxnLabel { "none" }; //!< lable of the coupled reaction, default value is none
    CoupledRxn coupledRxn;
    bool excludeVolumeBound { false }; //!< once two sites bound, still exclude their partners if this is set true

    std::vector<int> intProductList {}; //!< list of absolute interface state indices of the product(s)
    std::vector<int> intReactantList {}; //!< list of absolute interface state indices of the reactant(s)

    static unsigned numberOfRxns; //!< total number of unique ForwardRxns, CreateDestructRxns, and ConditionalRates
    static int totRxnSpecies; //!< total number of unique reactants and products

    std::vector<RxnIface> productListNew {}; //!< list of the product interfaces. indexed to  0 and 1, even for single
        //!< products (enters duplicate as dummy)
    std::vector<RxnIface> reactantListNew {}; //!< list of the reactant interfaces. indexed to 0 and 1, even for single
        //!< reactants (enters duplicate as dummy)

    std::vector<RateState> rateList {}; //!< list of rates for the reaction
    std::pair<RxnIface, RxnIface> stateChangeIface; //!< interfaces which don't change interaction but change state
    /**< Contains rates dependent on different parameters, e.g. bound interfaces, different states, etc.*/

    virtual void display() const = 0;

    /*
    Function serialize serializes the Molecule into arrayRank of bytes.
    */
    void serialize_base(unsigned char* arrayRank, int& nArrayRank) {
        // Serialization starts from the beginning of arrayRank
        PUSH(isObserved);
        serialize_string(observeLabel, arrayRank, nArrayRank);
        PUSH(loopCoopFactor);
        PUSH(bindRadSameCom);
        PUSH(isSymmetric);
        PUSH(isOnMem);
        PUSH(hasStateChange);
        PUSH(rxnType);
        PUSH(absRxnIndex);
        PUSH(relRxnIndex);
        PUSH(isCoupled);
        PUSH(length3Dto2D);

        serialize_string(rxnLabel, arrayRank, nArrayRank);
        serialize_string(coupledRxnLabel, arrayRank, nArrayRank);

        coupledRxn.serialize(arrayRank, nArrayRank);

        PUSH(excludeVolumeBound);

        // serialize intProductList vector of int
        serialize_primitive_vector<int>(intProductList, arrayRank, nArrayRank);
        serialize_primitive_vector<int>(intReactantList, arrayRank, nArrayRank);

        PUSH(numberOfRxns);
        PUSH(totRxnSpecies);

        // serialize productListNew vector of RxnIface
        serialize_abstract_vector<RxnIface>(productListNew, arrayRank, nArrayRank);
        serialize_abstract_vector<RxnIface>(reactantListNew, arrayRank, nArrayRank);
        serialize_abstract_vector<RateState>(rateList, arrayRank, nArrayRank);

        stateChangeIface.first.serialize(arrayRank, nArrayRank);
        stateChangeIface.second.serialize(arrayRank, nArrayRank);
        // std::cout << "Total serialize RxnBase size in bytes: " << nArrayRank <<
        // std::endl;
    }
    void deserialize_base(unsigned char* arrayRank, int& nArrayRank) {
        POP(isObserved);

        deserialize_string(observeLabel, arrayRank, nArrayRank);

        POP(loopCoopFactor);
        POP(bindRadSameCom);

        POP(isSymmetric);
        POP(isOnMem);
        POP(hasStateChange);

        POP(rxnType);

        POP(absRxnIndex);
        POP(relRxnIndex);
        POP(isCoupled);
        POP(length3Dto2D);

        deserialize_string(rxnLabel, arrayRank, nArrayRank);
        deserialize_string(coupledRxnLabel, arrayRank, nArrayRank);

        coupledRxn.deserialize(arrayRank, nArrayRank);

        POP(excludeVolumeBound);

        deserialize_primitive_vector<int>(intProductList, arrayRank, nArrayRank);
        deserialize_primitive_vector<int>(intReactantList, arrayRank, nArrayRank);

        POP(numberOfRxns);
        POP(totRxnSpecies);

        deserialize_abstract_vector<RxnIface>(productListNew, arrayRank, nArrayRank);
        deserialize_abstract_vector<RxnIface>(reactantListNew, arrayRank, nArrayRank);

        deserialize_abstract_vector<RateState>(rateList, arrayRank, nArrayRank);

        stateChangeIface.first.deserialize(arrayRank, nArrayRank);
        stateChangeIface.second.deserialize(arrayRank, nArrayRank);
        // std::cout << "Total deserialize RxnBase size in bytes: " << nArrayRank <<
        // std::endl;
    }
};

struct ParsedRxn; // forward declaration

/*!
 * \ingroup SimulClasses
 * \ingroup Reactions
 * \brief Holds information on one forward reaction
 *
 * This can be bimolecular or unimolecular.
 */
struct ForwardRxn : public RxnBase {
public:
    struct Angles {
        /*! \struct Angles
         * \ingroup Reactions
         * \ingroup Associate
         * \brief Data structure for holding the association angles of a bimolecular reaction.
         */

        double theta1; //!< reactant 1 center of mass-sigma angle
        double theta2; //!< reactant 2 center of mass-sigma angle
        double phi1; //!< reactant 1 sigma-center of mass-interface-norm dihedral
        double phi2; //!< reactant 1 sigma-center of mass-interface-norm dihedral
        double omega; //!< (reactant 1 center of mass)-(reactant1 interface)-(sigma)-(reactant 2 center of
            //!< mass)-(reactant 2
        //!< interface)

        // TODO: make it so that no angle can be -M_PI
        void display() const;

        explicit Angles() = default;
        explicit Angles(std::array<double, 5> angArray)
            : theta1(angArray[0])
            , theta2(angArray[1])
            , phi1(angArray[2])
            , phi2(angArray[3])
            , omega(angArray[4])
        {
        }
        explicit Angles(std::vector<double> angArray)
            : theta1(angArray[0])
            , theta2(angArray[1])
            , phi1(angArray[2])
            , phi2(angArray[3])
            , omega(angArray[4])
        {
        }
        Angles(double theta1, double theta2, double phi1, double phi2, double omega)
            : theta1(theta1)
            , theta2(theta2)
            , phi1(phi1)
            , phi2(phi2)
            , omega(omega)
        {
        }

        /*
        Function serialize serializes the Angles into array of bytes.
        */
        void serialize(unsigned char* arrayRank, int& nArrayRank) {
            PUSH(theta1);
            PUSH(theta2);
            PUSH(phi1);
            PUSH(phi2);
            PUSH(omega);
        }
        /*
        Function deserialize deserializes the Angles from given array of bytes.
        */
        void deserialize(unsigned char* arrayRank, int& nArrayRank) {
            POP(theta1);
            POP(theta2);
            POP(phi1);
            POP(phi2);
            POP(omega);
        }
    };

    bool isReversible { false }; //!< is the reaction reversible (does it have a back reaction)
    int conjBackRxnIndex { -1 }; //!< index of the ForwardRxn's BackRxn, if it is reversible
    bool irrevRingClosure { false }; //!< when true, reaction probability within sigma in same complex is unity
    std::vector<RxnBase>::iterator conjBackRxn; //!< iterator the reaction's BackRxn, if reversible (not implemented)
    std::string productName { "default" }; //!< the product as a string (just the reacting interfaces), for writing to species file
    //    std::string fullProductName {}; //!< the full product as written in the parameters file

    // arrays for association
    double bindRadius { 1.0 };
    double bindRadius2D { 1.0 };

    Vector norm1 { 0, 0, 1 }; //!< arbitrary 'normal' vector determined from an ancillary interface for reactant 1
    Vector norm2 { 0, 0, 1 }; //!< arbitrary 'normal' vector determined from an ancillary interface for reactant 2

    Angles assocAngles { std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN() }; //!< Angles relative to sigma for association

    void display() const override;
    void assoc_display(const std::vector<MolTemplate>& molTemplateList) const;
    ForwardRxn bngl_copy_rxn();

    // constructors
    ForwardRxn() = default;
    /*!
     * \brief This constructor takes a fully parsedRxn and converts it into a ForwardRxn for use during the
     simulation.
     */
    explicit ForwardRxn(ParsedRxn& parsedRxn, const std::vector<MolTemplate>& molTemplateList);
    ForwardRxn(double bindRadius, const Angles& assocAngles)
        : bindRadius(bindRadius)
        , assocAngles(assocAngles)
    {
    }
    ForwardRxn(
        double bindRadius, const Angles& assocAngles, const double& sigma, const Vector& norm1, const Vector& norm2)
        : bindRadius(bindRadius)
        , norm1(norm1)
        , norm2(norm2)
        , assocAngles(assocAngles)
    {
    }

    /*
    Function serialize serializes the ForwardRxn into array of bytes.
    */
    void serialize(unsigned char* arrayRank, int& nArrayRank) {
        // std::cout << "+FwdRnx serialization starts here..." << std::endl;
        this->serialize_base(arrayRank, nArrayRank);

        PUSH(isReversible);
        PUSH(conjBackRxnIndex);
        PUSH(irrevRingClosure);
        // conjBackRxn - vector_.begin()
        // std::vector<RxnBase>::iterator conjBackRxn; //!< iterator the reaction's
        // BackRxn, if reversible (not implemented)
        serialize_string(productName, arrayRank, nArrayRank);
        PUSH(bindRadius);
        PUSH(bindRadius2D);

        norm1.serialize(arrayRank, nArrayRank);
        norm2.serialize(arrayRank, nArrayRank);
        assocAngles.serialize(arrayRank, nArrayRank);
    }
    void deserialize(unsigned char* arrayRank, int& nArrayRank) {
        this->deserialize_base(arrayRank, nArrayRank);

        POP(isReversible);
        POP(conjBackRxnIndex);
        POP(irrevRingClosure);
        // std::vector<RxnBase>::iterator conjBackRxn; //!< iterator the reaction's
        // BackRxn, if reversible (not implemented)
        deserialize_string(productName, arrayRank, nArrayRank);
        POP(bindRadius);
        POP(bindRadius2D);

        norm1.deserialize(arrayRank, nArrayRank);
        norm2.deserialize(arrayRank, nArrayRank);
        assocAngles.deserialize(arrayRank, nArrayRank);
    }
};

/*******************/

struct BackRxn : public RxnBase {
    /*! \struct BackRxn
     * \ingroup SimulClasses
     * \ingroup Reactions
     * \brief Holds information on the back reaction counterpart to a reversible ForwardRxn
     *
     * This can be bimolecular or unimolecular
     */

    size_t conjForwardRxnIndex { 0 }; //!< the index of this reaction's ForwardRxn counterpart
    std::vector<RxnBase>::iterator conjForwardRxn; //!< iterator to this reaction's ForwardRxn (not implemented)

    void display() const override;

    BackRxn() = default;

    /*!
     * \brief This constructor creates a conjugate BackRxn from a reversible ForwardRxn
     */
    explicit BackRxn(double offRatekb, ForwardRxn& forwardRxn);

    void serialize(unsigned char* arrayRank, int& nArrayRank) {
        this->serialize_base(arrayRank, nArrayRank);

        PUSH(conjForwardRxnIndex);
    }
    void deserialize(unsigned char* arrayRank, int& nArrayRank) {
        this->deserialize_base(arrayRank, nArrayRank);

        POP(conjForwardRxnIndex);
    }
};

/*******************/

struct CreateDestructRxn : public RxnBase {
    /*! \struct CreateDestructRxnNew
     * \ingroup SimulClasses
     * \ingroup Reactions
     * \brief Holds information on creation/destruction reactions.
     *
     * Possible reactions:
     *   - Creation from concentration: 0 -> X
     *   - Creation from molecule: X -> X + Y
     *   - Destruction: X -> 0
     *
     * ## Limitations
     *   - Currently only allows the creation of the default product from MolTemplate
     *   - Involved Molecules which have Interface States must have them explicitly included.
     *   - The new Molecule must be listed second.
     */

    struct CreateDestructMol {
        int molTypeIndex { -1 };
        std::string molName {};
        std::vector<RxnIface> interfaceList;

        CreateDestructMol() = default;
        CreateDestructMol(int _molTypeIndex, const std::vector<RxnIface>& _interfaceList)
            : molTypeIndex(_molTypeIndex)
            , interfaceList(_interfaceList)
        {
        }

        void serialize(unsigned char* arrayRank, int& nArrayRank) {
            PUSH(molTypeIndex);
            serialize_string(molName, arrayRank, nArrayRank);
            serialize_abstract_vector<RxnIface>(interfaceList, arrayRank, nArrayRank);
        }
        void deserialize(unsigned char* arrayRank, int& nArrayRank) {
            POP(molTypeIndex);
            deserialize_string(molName, arrayRank, nArrayRank);
            deserialize_abstract_vector<RxnIface>(interfaceList, arrayRank, nArrayRank);
        }
    };

    std::vector<CreateDestructMol> reactantMolList {};
    std::vector<CreateDestructMol> productMolList;
    unsigned conjBackRxnIndex { 0 };
    double creationRadius { 1.0 }; //!< the radius of the sphere within which the created specie can be placed

    void display() const override;

    /* CONSTRUCTORS */
    CreateDestructRxn() = default;

    /*!
     * \brief This function takes a fully parsed reaction block and converts it to a CreateDestructRxn for use
     * during the simulation
     */
    explicit CreateDestructRxn(ParsedRxn& parsedRxn, const std::vector<MolTemplate>& molTemplateList);

    void serialize(unsigned char* arrayRank, int& nArrayRank) {
        // std::cout << "+Molecule serialization starts here..." << std::endl;
        this->serialize_base(arrayRank, nArrayRank);

        serialize_abstract_vector<CreateDestructMol>(reactantMolList, arrayRank, nArrayRank);
        serialize_abstract_vector<CreateDestructMol>(productMolList, arrayRank, nArrayRank);
        PUSH(conjBackRxnIndex);
        PUSH(creationRadius);
    }
    void deserialize(unsigned char* arrayRank, int& nArrayRank) {
        this->deserialize_base(arrayRank, nArrayRank);

        deserialize_abstract_vector<CreateDestructMol>(reactantMolList, arrayRank, nArrayRank);
        deserialize_abstract_vector<CreateDestructMol>(productMolList, arrayRank, nArrayRank);
        POP(conjBackRxnIndex);
        POP(creationRadius);
    }
};

struct TransmissionRxn : public RxnBase {
    /*! \struct TransmissionRxn
     * \ingroup SimulClasses
     * \ingroup Reactions
     * \brief Holds information on the transmission reaction counterpart to a parseRxn
     *
     */


    /*!
     * \brief This constructor creates TransmissionRxn from a parseRxn
     */

    struct TransmissionMol {
        int molTypeIndex { -1 };
        std::string molName {};
        std::vector<RxnIface> interfaceList;

        TransmissionMol() = default;
        TransmissionMol(int _molTypeIndex, const std::vector<RxnIface>& _interfaceList)
            : molTypeIndex(_molTypeIndex)
            , interfaceList(_interfaceList)
        {
        }
    };

    std::vector<TransmissionMol> reactantMolList {};
    std::vector<TransmissionMol> productMolList;
    double bindRadius { 1 };
    
    void display() const override;
    TransmissionRxn() = default;
    explicit TransmissionRxn(ParsedRxn& parsedRxn, std::vector<MolTemplate>& molTemplateList, bool isForward, int rxnIndex);
    
};