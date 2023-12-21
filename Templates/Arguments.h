#ifndef Arguments_h
#define Arguments_h

namespace arguments
{
    enum class MeasureProperty
    {
        One = 0,
        Two = 1,
        Three = 2
    };

    bool CheckProperty(MeasureProperty iProperty, MeasureProperty Property)
    {
        return iProperty == Property;
    }

    template<typename... Targs>
    bool CheckProperty(MeasureProperty iProperty, MeasureProperty Property, Targs... types)
    {
        return CheckProperty(iProperty, Property) || CheckProperty(iProperty, types...);
    }
}

#endif /* Arguments_h */
