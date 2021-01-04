
#include "OutputData.hpp"
#include "Generator.hpp"

String OutputData::ParagraphSegment::generate(Generator& generator) const {return generator.generate(*this);}
String OutputData::TitleSegment::generate(Generator& generator) const {return generator.generate(*this);}
String OutputData::SeparatorSegment::generate(Generator& generator) const {return generator.generate(*this);}
String OutputData::FigureSegment::generate(Generator& generator) const {return generator.generate(*this);}
String OutputData::RuleSegment::generate(Generator& generator) const {return generator.generate(*this);}
String OutputData::BulletListSegment::generate(Generator& generator) const {return generator.generate(*this);}
String OutputData::NumberedListSegment::generate(Generator& generator) const {return generator.generate(*this);}
String OutputData::BlockquoteSegment::generate(Generator& generator) const {return generator.generate(*this);}
String OutputData::EnvironmentSegment::generate(Generator& generator) const {return generator.generate(*this);}
String OutputData::TableSegment::generate(Generator& generator) const {return generator.generate(*this);}
String OutputData::TexSegment::generate(Generator& generator) const {return generator.generate(*this);}
String OutputData::TexPartSegment::generate(Generator& generator) const {return generator.generate(*this);}
String OutputData::PdfSegment::generate(Generator& generator) const {return generator.generate(*this);}
