#include "src/utility/solver.h"

#include "src/settings/SettingsManager.h"

#include "src/solver/NativeLinearEquationSolver.h"
#include "src/solver/GmmxxLinearEquationSolver.h"

#include "src/solver/NativeNondeterministicLinearEquationSolver.h"
#include "src/solver/GmmxxNondeterministicLinearEquationSolver.h"

#include "src/solver/GurobiLpSolver.h"
#include "src/solver/GlpkLpSolver.h"

namespace storm {
    namespace utility {
        namespace solver {
            std::shared_ptr<storm::solver::LpSolver> getLpSolver(std::string const& name) {
                std::string const& lpSolver = storm::settings::SettingsManager::getInstance()->getOptionByLongName("lpsolver").getArgument(0).getValueAsString();
                if (lpSolver == "gurobi") {
                    return std::shared_ptr<storm::solver::LpSolver>(new storm::solver::GurobiLpSolver(name));
                } else if (lpSolver == "glpk") {
                    return std::shared_ptr<storm::solver::LpSolver>(new storm::solver::GlpkLpSolver(name));
                }
                
                throw storm::exceptions::InvalidSettingsException() << "No suitable LP solver selected.";
            }
            
            template<typename ValueType>
            std::shared_ptr<storm::solver::LinearEquationSolver<ValueType>> getLinearEquationSolver() {
                std::string const& linearEquationSolver = storm::settings::SettingsManager::getInstance()->getOptionByLongName("linsolver").getArgument(0).getValueAsString();
                if (linearEquationSolver == "gmm++") {
                    return std::shared_ptr<storm::solver::LinearEquationSolver<ValueType>>(new storm::solver::GmmxxLinearEquationSolver<ValueType>());
                } else if (linearEquationSolver == "native") {
                    return std::shared_ptr<storm::solver::LinearEquationSolver<ValueType>>(new storm::solver::NativeLinearEquationSolver<ValueType>());
                }
                
                throw storm::exceptions::InvalidSettingsException() << "No suitable linear equation solver selected.";
            }
            
            template<typename ValueType>
            std::shared_ptr<storm::solver::NondeterministicLinearEquationSolver<ValueType>> getNondeterministicLinearEquationSolver() {
                std::string const& nondeterministicLinearEquationSolver = storm::settings::SettingsManager::getInstance()->getOptionByLongName("ndsolver").getArgument(0).getValueAsString();
                if (nondeterministicLinearEquationSolver == "gmm++") {
                    return std::shared_ptr<storm::solver::NondeterministicLinearEquationSolver<ValueType>>(new storm::solver::GmmxxNondeterministicLinearEquationSolver<ValueType>());
                } else if (nondeterministicLinearEquationSolver == "native") {
                    return std::shared_ptr<storm::solver::NondeterministicLinearEquationSolver<ValueType>>(new storm::solver::NativeNondeterministicLinearEquationSolver<ValueType>());
                }
                
                throw storm::exceptions::InvalidSettingsException() << "No suitable nondeterministic linear equation solver selected.";
            }
            
            template std::shared_ptr<storm::solver::LinearEquationSolver<double>> getLinearEquationSolver();

            template std::shared_ptr<storm::solver::NondeterministicLinearEquationSolver<double>> getNondeterministicLinearEquationSolver();
        }
    }
}