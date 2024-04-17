#pragma once

#include "customTypes/FinalIK/Point.hpp"

namespace VRMQavatars::FinalIK {
    void Point::FixTransforms() {
        if (this->defaultLocalPosition != transform->localPosition)
        {
            transform->localPosition = this->defaultLocalPosition;
        }
        if (this->defaultLocalRotation != transform->localRotation)
        {
            transform->localRotation = this->defaultLocalRotation;
        }
    }

    void Point::StoreDefaultLocalState() {
        defaultLocalPosition = transform->localPosition;
	    defaultLocalRotation = transform->localRotation;
    }

    void Point::UpdateSolverLocalPosition() {
        solverPosition = transform->localPosition;
    }

    void Point::UpdateSolverLocalState() {
        solverPosition = transform->localPosition;
        solverRotation = transform->localRotation;
    }

    void Point::UpdateSolverPosition() {
        solverPosition = transform->position;
    }

    void Point::UpdateSolverState() {
        solverPosition = transform->position;
        solverRotation = transform->rotation;
    }
}