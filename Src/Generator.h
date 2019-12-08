
#pragma once

#include <nstd/String.h>

#include "OutputData.h"

class Generator
{
public:
    virtual String generate(const OutputData::ParagraphSegment& segment) = 0;
    virtual String generate(const OutputData::TitleSegment& segment) = 0;
    virtual String generate(const OutputData::SeparatorSegment& segment) = 0;
    virtual String generate(const OutputData::FigureSegment& segment) = 0;
    virtual String generate(const OutputData::RuleSegment& segment) = 0;
    virtual String generate(const OutputData::BulletListSegment& segment) = 0;
    virtual String generate(const OutputData::NumberedListSegment& segment) = 0;
    virtual String generate(const OutputData::BlockquoteSegment& segment) = 0;
    virtual String generate(const OutputData::EnvironmentSegment& segment) = 0;
    virtual String generate(const OutputData::TableSegment& segment) = 0;
    virtual String generate(const OutputData::TexSegment& segment) = 0;
    virtual String generate(const OutputData::TexPartSegment& segment) = 0;
    virtual String generate(const OutputData::PdfSegment& segment) = 0;
};
