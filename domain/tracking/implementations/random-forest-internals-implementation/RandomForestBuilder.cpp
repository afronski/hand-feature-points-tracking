#include "../../../common/debug.hpp"

#include "RandomForestBuilder.hpp"

RandomForestBuilder::RandomForestBuilder(
                        const FeaturesCollection& features,
                        const ClassificatorParameters& parameters):
  featuresCollection(features),
  classificatorParameters(parameters)
{}

void RandomForestBuilder::build() {

}