// Private unity fragment; included by AC4Tools.cpp inside its namespace.
void EmitJump(std::uint8_t*& p, std::uintptr_t target) {
    *p++ = 0xE9;
    *reinterpret_cast<std::int32_t*>(p) =
        static_cast<std::int32_t>(reinterpret_cast<std::uint8_t*>(target) - p - 4);
    p += 4;
}

void EmitCall(std::uint8_t*& p, const void* target) {
    *p++ = 0xE8;
    *reinterpret_cast<std::int32_t*>(p) =
        static_cast<std::int32_t>(reinterpret_cast<const std::uint8_t*>(target) - p - 4);
    p += 4;
}
