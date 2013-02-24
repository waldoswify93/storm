/*
 * BinaryExpression.h
 *
 *  Created on: 27.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_EXPRESSIONS_BINARYEXPRESSION_H_
#define STORM_IR_EXPRESSIONS_BINARYEXPRESSION_H_

#include "BaseExpression.h"
#include <memory>

namespace storm {

namespace ir {

namespace expressions {

class BinaryExpression : public BaseExpression {
public:
	BinaryExpression(ReturnType type, std::shared_ptr<BaseExpression> left, std::shared_ptr<BaseExpression> right) : BaseExpression(type), left(left), right(right) {

	}

	std::shared_ptr<BaseExpression> const& getLeft() const {
		return left;
	}

	std::shared_ptr<BaseExpression> const& getRight() const {
		return right;
	}

private:
	std::shared_ptr<BaseExpression> left;
	std::shared_ptr<BaseExpression> right;
};

} // namespace expressions

} // namespace ir

} // namespace storm

#endif /* STORM_IR_EXPRESSIONS_BINARYEXPRESSION_H_ */
