/*! \file creation.hpp

 * ### Created on 2019-01-23 by Matthew Varga
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

#include "classes/class_Observable.hpp"
#include "classes/class_Rxns.hpp"
#include "classes/class_SimulVolume.hpp"
#include "classes/class_copyCounters.hpp"

/*!
 * \brief Create a new Molecule from a CreateDestructRxn.
 *
 * \param[in] molType Identifying integer (index) of the MolTemplate corresponding to the Molecule to be created
 * \param[in] params Simulation Parameters as provided by user.
 * \param[in] molTemplate The MolTemplate which the created Molecule identifies as
 * \param[in] moleculeList List of all Molecules in the system.
 * \param[in] complexList List of all Complexes in the system.
 *
 * TODO: currently randomly places molecule product of unimolecular creation reaction. change to right next to reactant?
 */
void create_molecule_and_complex_from_rxn(int parentMolIndex, int& newMolIndex, int& newComIndex, bool createInVicinity,
    MolTemplate& createdMolTemp, Parameters& params,
    const CreateDestructRxn& currRxn, SimulVolume& simulVolume,
    std::vector<Molecule>& moleculeList, std::vector<Complex>& complexList,
    std::vector<MolTemplate>& molTemplateList,
    const std::vector<ForwardRxn>& forwardRxns, const Membrane& membraneObject);

void create_molecule_and_complex_from_transmission_rxn(int parentMolIndex, int &newMolIndex, int &newComIndex,
                                                        MolTemplate &createdMolTemp, Parameters &params, const TransmissionRxn &currRxn, SimulVolume &simulVolume,
                                                        std::vector<Molecule> &moleculeList, std::vector<Complex> &complexList,
                                                        std::vector<MolTemplate> &molTemplateList, const std::vector<ForwardRxn> &forwardRxns, const Membrane &membraneObject,
                                                        const Coord &newPos);

void check_for_zeroth_order_creation(unsigned simItr, Parameters& params, SimulVolume& simulVolume,
    const std::vector<ForwardRxn>& forwardRxns,
    const std::vector<CreateDestructRxn>& createDestructRxns,
    std::vector<Molecule>& moleculeList, std::vector<Complex>& complexList,
    std::vector<MolTemplate>& molTemplateList,
    std::map<std::string, int>& observablesList, copyCounters& counterArrays, Membrane& membraneObject);

void check_for_unimolecular_reactions(unsigned simItr, Parameters& params, std::vector<Molecule>& moleculeList, std::vector<Complex>& complexList, SimulVolume& simulVolume,
    const std::vector<ForwardRxn>& forwardRxns, const std::vector<BackRxn>& backRxns, const std::vector<CreateDestructRxn>& createDestructRxns,
    std::vector<MolTemplate>& molTemplateList, std::map<std::string, int>& observablesList, copyCounters& counterArrays, Membrane& membraneObject, std::vector<double>& IL2DbindingVec,
    std::vector<double>& IL2DUnbindingVec, std::vector<double>& ILTableIDs, std::ofstream& assocDissocFile);
void check_for_unimolstatechange_reactions(unsigned simItr, Parameters& params, std::vector<Molecule>& moleculeList, std::vector<Complex>& complexList,
    SimulVolume& simulVolume, const std::vector<ForwardRxn>& forwardRxns, const std::vector<BackRxn>& backRxns,
    const std::vector<CreateDestructRxn>& createDestructRxns, std::vector<MolTemplate>& molTemplateList,
    std::map<std::string, int>& observablesList, copyCounters& counterArrays, Membrane& membraneObject);
void check_for_unimolecular_reactions_population(long long int simItr, Parameters& params, std::vector<Molecule>& moleculeList, std::vector<Complex>& complexList,
    SimulVolume& simulVolume, const std::vector<ForwardRxn>& forwardRxns, const std::vector<BackRxn>& backRxns,
    const std::vector<CreateDestructRxn>& createDestructRxns, std::vector<MolTemplate>& molTemplateList,
    std::map<std::string, int>& observablesList, copyCounters& counterArrays, Membrane& membraneObject, std::ofstream& assocDissocFile);

void check_for_destruction(unsigned simItr, const Parameters& params, const std::vector<CreateDestructRxn>& createDestructRxns,
    const std::vector<Molecule>& moleculeList, const std::vector<Complex>& complexList,
    const std::vector<MolTemplate>& molTemplateList);

void correct_structure(const std::vector<Molecule> &moleculeList, Complex &complex,
                       const std::vector<ForwardRxn> &forwardRxns);

bool break_interaction(long long int iter, size_t relIface1, size_t relIface2, Molecule& reactMol1, Molecule& reactMol2,
// <<<<<<< HEAD
//     const BackRxn& currRxn, const std::vector<ForwardRxn> &forwardRxns, std::vector<Molecule>& moleculeList,
//     std::vector<Complex>& complexList, std::vector<MolTemplate>& molTemplateList, int ILindexMol);
// =======
    const BackRxn& currRxn, std::vector<Molecule>& moleculeList,
    std::vector<Complex>& complexList, std::vector<MolTemplate>& molTemplateList, int ILindexMol, const ForwardRxn &conjForwardRxn, 
    bool &breakLinkComplex, double timeStep, std::ofstream& assocDissocFile);
// >>>>>>> master

bool determine_parent_complex(int pro1Index, int pro2Index, int newComIndex, std::vector<Molecule>& moleculeList,
    std::vector<Complex>& complexList);

bool determine_parent_complex_IL(int pro1Index, int pro2Index, int newComIndex, std::vector<Molecule>& moleculeList,
    std::vector<Complex>& complexList, int ILindexMol);
/*!
 * \brief Initializes a Molecule with random coordinates within the simulation volume.
 *
 * Used for zeroth order creation reactions.
 */
Molecule initialize_molecule_after_zeroth_reaction(
    int index, Parameters& params, MolTemplate& molTemplate, const CreateDestructRxn& currRxn, const Membrane& membraneObject);

/*! \ingroup Reactions
 * \brief Initializes a molecule created from a unimolecular reaction (i.e. by another Molecule)
 *
 * Creates the Molecule with random coordinates in a sphere with a user-defined radius around the parent Molecule, using
 *
 * \f$ \theta = RN \times 2\pi\f$
 * \f$ \phi = \acos(RN \times 2 - 1)\f$
 * transVec = \sigma\f$(\cos\theta\sin\phi,\sin\theta\sin\phi,\cos\phi)\f$
 */
Molecule initialize_molecule_after_uni_reaction(int index, const Molecule& parentMol, Parameters& params,
    MolTemplate& molTemplate, const CreateDestructRxn& currRxn);

Molecule initialize_molecule_after_transmission_reaction(int index, const Molecule& parentMol, Parameters& params,
														 MolTemplate& molTemplate, const TransmissionRxn& currRxn, const Coord& newPos,
                                                         const bool plusRand, const Membrane& membraneObject);

bool moleculeOverlaps(const Parameters &params, SimulVolume &simulVolume, Molecule &createdMol,
                      std::vector<Molecule> &moleculeList, std::vector<Complex> &complexList, const std::vector<ForwardRxn> &forwardRxns,
                      const std::vector<MolTemplate> &molTemplateList, const Membrane &membraneObject);

void check_dissociation(unsigned int simItr, const Parameters& params, SimulVolume& simulVolume,
    std::vector<MolTemplate>& molTemplateList, std::map<std::string, int>& observablesList, unsigned int molItr,
    std::vector<Molecule>& moleculeList,
    std::vector<Complex>& complexList, const std::vector<BackRxn>& backRxns, const std::vector<ForwardRxn>& forwardRxns,
    const std::vector<CreateDestructRxn>& createDestructRxns, copyCounters& counterArrays, const Membrane& membraneObject, std::ofstream& assocDissocFile);

void perform_transmission_reaction(int moleculeIndex, std::vector<Molecule> &moleculeList, std::vector<Complex> &complexList,
                                   std::vector<MolTemplate> &molTemplateList, Membrane &membraneObject, copyCounters &counterArrays, Parameters &params,
                                   std::vector<TransmissionRxn> &transmissionRxns, const std::vector<ForwardRxn> &forwardRxns, SimulVolume &simulVolume);

void check_compartment_reaction(int pro1Index, int pro2Index, int simItr,
                                const Parameters &params, std::vector<Molecule> &moleculeList, std::vector<Complex> &complexList,
                                const std::vector<MolTemplate> &molTemplateList, const std::vector<TransmissionRxn> &transmissionRxns,
                                const std::vector<BackRxn> &backRxns, copyCounters &counterArrays, Membrane &membraneObject, std::vector<double> &IL2DbindingVec,
                                std::vector<double> &IL2DUnbindingVec, std::vector<double> &ILTableIDs);
