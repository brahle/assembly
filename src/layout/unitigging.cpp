#include "unitigging.h"

namespace layout {

Unitigging::Unitigging(const overlap::ReadSet& reads) :reads_(reads) {}

Unitigging::~Unitigging() {}

void Unitigging::start() {
  removeContainmentEdges();
}

void Unitigging::removeContainmentEdges() {
}

void Unitigging::removeTransitiveEdges() {
}

void Unitigging::connectEdges() {
}

};

