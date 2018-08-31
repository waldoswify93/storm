//
// Created by Jip Spel on 28.08.18.
//

#ifndef STORM_LATTICEEXTENDER_H
#define STORM_LATTICEEXTENDER_H

#include <storm/logic/Formula.h>
#include "storm/models/sparse/Dtmc.h"
#include "storm-pars/analysis/Lattice.h"
#include "storm/api/storm.h"




namespace storm {
    namespace analysis {


        template<typename SparseModelType>
        class LatticeExtender {

        public:
            LatticeExtender(std::shared_ptr<SparseModelType> model);

            storm::analysis::Lattice* toLattice(std::vector<std::shared_ptr<storm::logic::Formula const>> formulas);

            storm::analysis::Lattice* extendLattice(storm::analysis::Lattice* lattice, std::shared_ptr<storm::expressions::ExpressionManager> expressionManager, std::set<storm::expressions::BinaryRelationExpression*> assumptions);

        private:
            std::shared_ptr<SparseModelType> model;

            std::map<uint_fast64_t, storm::storage::BitVector> stateMap;

            storm::analysis::Lattice* extendLattice(storm::analysis::Lattice* lattice);

        };
    }
}

#endif //STORM_LATTICEEXTENDER_H
