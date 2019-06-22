namespace Zuazo {

constexpr PixelComponent::PixelComponent() :
    type(NONE),
    depth(0),
    plane(0),
    subsampling(NO_SUBSAMPLING),
    flags(0)
{
}

constexpr PixelComponent::PixelComponent(uint depth) :
    type(NONE),
    depth(depth),
    plane(0),
    subsampling(NO_SUBSAMPLING),
    flags(0)
{
}

constexpr PixelComponent::PixelComponent(PixelComponentType type, uint depth, uint plane, const Subsampling& subs, Flags flags) :
    type(type),
    depth(depth),
    plane(plane),
    subsampling(subs),
    flags(flags)
{
}

constexpr PixelComponent::PixelComponent(const PixelComponent& other, uint plane, const Subsampling& subs) : 
    type(other.type),
    depth(other.depth),
    plane(plane),
    subsampling(subs),
    flags(other.flags)
{
}

constexpr unsigned long long PixelComponent::flag_bit(Flag x){
    return 1 << static_cast<uint>(x);
}

constexpr PixelComponent::operator bool() const{
    return depth;
}

constexpr PixelFormat::PixelFormat(std::initializer_list<PixelComponent> components) :
    m_components(components)
{
}

constexpr const ComponentArray& PixelFormat::getComponents() const{
    return m_components;
}

constexpr uint PixelFormat::getComponentCount(){
    uint count = 0;

    for(auto ite = m_components.cbegin(); ite != m_components.cend(); ++ite) const{
        ++count;
    }

    return count;
}

constexpr Math::Rational_t PixelFormat::getSize() const;
    Math::Rational_t size = 0;

    for(auto ite = m_components.cbegin(); ite != m_components.cend(); ++ite){
        size += ite->depth / (ite->subsampling.x * ite->subsampling.y);
    }
    
    return size;
}

constexpr bool PixelFormat::hasColor() const;
    bool has_color = false;

    for(auto ite = m_components.cbegin(); ite != m_components.cend(); ++ite){
        if(ite->type != PixelComponent::ALPHA && ite->type != PixelComponent::LUMINANCE){
            has_color = true;
            break;
        }
    }
    
    return has_color;
}

constexpr bool PixelFormat::hasAlpha() const;
    bool has_alpha = false;

    for(auto ite = m_components.cbegin(); ite != m_components.cend(); ++ite){
        if(ite->type == PixelComponent::ALPHA){
            has_alpha = true;
            break;
        }
    }
    
    return has_alpha;
}