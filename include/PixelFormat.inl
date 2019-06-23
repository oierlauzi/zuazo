namespace Zuazo {

constexpr PixelComponent::Subsampling::Subsampling() :
    x(1, 1),
    y(1, 1)
{
}

constexpr PixelComponent::PixelComponent() :
    type(NONE),
    depth(0),
    plane(0),
    subsampling(),
    flags(0)
{
}

constexpr PixelComponent::PixelComponent(uint depth) :
    type(NONE),
    depth(depth),
    plane(0),
    subsampling(),
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







constexpr PixelFormat::operator bool() const{
    bool result = false;

    for(auto ite = cbegin(); ite != cend(); ++ite){
        if(*ite){
            result = true;
            break;
        }
    }
    
    return result;
}

constexpr uint PixelFormat::getComponentCount() const{
    uint count = 0;

    for(auto ite = cbegin(); ite != cend(); ++ite){
        ++count;
    }

    return count;
}

constexpr bool PixelFormat::hasColor() const{
    bool has_color = false;

    for(auto ite = cbegin(); ite != cend(); ++ite){
        if(ite->type && ite->type != PixelComponent::ALPHA && ite->type != PixelComponent::LUMINANCE){
            has_color = true;
            break;
        }
    }
    
    return has_color;
}

constexpr bool PixelFormat::hasAlpha() const{
    bool has_alpha = false;

    for(auto ite = cbegin(); ite != cend(); ++ite){
        if(ite->type == PixelComponent::ALPHA){
            has_alpha = true;
            break;
        }
    }
    
    return has_alpha;
}

}