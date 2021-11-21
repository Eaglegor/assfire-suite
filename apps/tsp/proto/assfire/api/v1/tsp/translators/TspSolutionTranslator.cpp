#include "TspSolutionTranslator.hpp"
#include "TspValidationResultTranslator.hpp"
#include <iterator>
#include "TspCostTranslator.hpp"

namespace assfire::api::v1::tsp {
    TspSolutionTranslator::ApiTspSolution TspSolutionTranslator::fromProto(const TspSolutionTranslator::ProtoTspSolution &value) {
        ApiTspSolution::Sequence optimized_sequence;
        optimized_sequence.reserve(value.optimized_sequence_size());
        std::copy(value.optimized_sequence().begin(), value.optimized_sequence().end(), std::back_inserter(optimized_sequence));
        return ApiTspSolution(optimized_sequence, TspCostTranslator::fromProto(value.cost()), TspValidationResultTranslator::fromProto(value.validation_result()), value.is_final());
    }

    TspSolutionTranslator::ProtoTspSolution TspSolutionTranslator::toProto(const TspSolutionTranslator::ApiTspSolution &value) {
        ProtoTspSolution result;
        for(int id : value.getSequence()) {
            result.add_optimized_sequence(id);
        }
        result.mutable_cost()->CopyFrom(TspCostTranslator::toProto(value.getCost()));
        result.mutable_validation_result()->CopyFrom(TspValidationResultTranslator::toProto(value.getValidationResult()));
        result.set_is_final(value.isFinalSolution());
        return result;
    }
}
