/*
 * BoundedUntil.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef BOUNDEDUNTIL_H_
#define BOUNDEDUNTIL_H_

#include "PCTLPathFormula.h"
#include "PCTLStateFormula.h"
#include "boost/integer/integer_mask.hpp"
#include "boost/lexical_cast.hpp"
#include <string>

namespace mrmc {

namespace formula {

/*!
 * @brief
 * Class for a PCTL (path) formula tree with a BoundedUntil node as root.
 *
 * Has two PCTL state formulas as sub formulas/trees.
 *
 * @par Semantics
 * The formula holds iff in at most \e bound steps, formula \e right (the right subtree) holds, and before,
 * \e left holds.
 *
 * The subtrees are seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 * @see PCTLPathFormula
 * @see PCTLFormula
 */
template <class T>
class BoundedUntil : public PCTLPathFormula<T> {

public:
	/*!
	 * Empty constructor
	 */
	BoundedUntil() {
		this->left = NULL;
		this->right = NULL;
		bound = 0;
	}

	/*!
	 * Constructor
	 *
	 * @param left The left formula subtree
	 * @param right The left formula subtree
	 * @param bound The maximal number of steps
	 */
	BoundedUntil(PCTLStateFormula<T>* left, PCTLStateFormula<T>* right,
					 uint_fast64_t bound) {
		this->left = left;
		this->right = right;;
		this->bound = bound;
	}

	/*!
	 * Destructor.
	 *
	 * Also deletes the subtrees.
	 * (this behaviour can be prevented by setting the subtrees to NULL before deletion)
	 */
	virtual ~BoundedUntil() {
	  if (left != NULL) {
		  delete left;
	  }
	  if (right != NULL) {
		  delete right;
	  }
	}

	/*!
	 * Sets the left child node.
	 *
	 * @param newLeft the new left child.
	 */
	void setLeft(PCTLStateFormula<T>* newLeft) {
		left = newLeft;
	}

	/*!
	 * Sets the right child node.
	 *
	 * @param newRight the new right child.
	 */
	void setRight(PCTLStateFormula<T>* newRight) {
		right = newRight;
	}

	/*!
	 * @returns a pointer to the left child node
	 */
	PCTLStateFormula<T>& getLeft() {
		return *left;
	}

	/*!
	 * @returns a pointer to the right child node
	 */
	PCTLStateFormula<T>& getRight() {
		return *right;
	}

	/*!
	 * @returns the maximally allowed number of steps for the bounded until operator
	 */
	uint_fast64_t getBound() {
		return bound;
	}

	/*!
	 * Sets the maximally allowed number of steps for the bounded until operator
	 *
	 * @param bound the new bound.
	 */
	void setBound(uint_fast64_t bound) {
		this->bound = bound;
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() {
		std::string result = "(";
		result += left->toString();
		result += " U<=";
		result +=  boost::lexical_cast<std::string>(bound);
		result += " ";
		result += right->toString();
		result += ")";
		return result;
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new BoundedUntil-object that is identical the called object.
	 */
	virtual PCTLPathFormula<T>* clone() {
		BoundedUntil<T>* result = new BoundedUntil();
		result->setBound(bound);
		if (left != NULL) {
			result->setLeft(left->clone());
		}
		if (right != NULL) {
			result->setRight(right->clone());
		}
		return result;
	}


	/*!
	 * Calls the model checker to check this formula.
	 * Needed to infer the correct type of formula class.
	 *
	 * @note This function should only be called in a generic check function of a model checker class. For other uses,
	 *       the methods of the model checker should be used.
	 *
	 * @returns A vector indicating the probability that the formula holds for each state.
	 */
	virtual std::vector<T> *check(mrmc::modelChecker::DtmcPrctlModelChecker<T>& modelChecker) {
	  return modelChecker.checkBoundedUntil(this);
	}

private:
	PCTLStateFormula<T>* left;
	PCTLStateFormula<T>* right;
	uint_fast64_t bound;
};

} //namespace formula

} //namespace mrmc

#endif /* BOUNDEDUNTIL_H_ */
