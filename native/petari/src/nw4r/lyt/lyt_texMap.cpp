#include "nw4r/lyt/common.h"
<<<<<<< HEAD
#include "nw4r/lyt/texMap.h"
#include <revolution/gx/GXEnum.h>
#include <revolution/gx/GXGet.h>
#include <revolution/gx/GXStruct.h>
#include <revolution/tpl.h>
=======
#include <JSystem/J3DGraphBase/J3DNativeTexture.hpp>
#include "nw4r/lyt/texMap.h"
#include <revolution/gx/GXEnum.h>
#include <revolution/gx/GXStruct.h>
#include "JSystem/JUtility/JUTException.hpp"
>>>>>>> upstream/main

namespace nw4r {
    namespace lyt {
        void TexMap::Get(_GXTexObj* pTexObj) const {
            if (detail::IsCITexelFormat(GetTexelFormat())) {
<<<<<<< HEAD
                u32 tlutName = GXGetTexObjTlut(pTexObj);
                GXInitTexObjCI(pTexObj, mImage, mWidth, mHeight, GXCITexFmt(GetTexelFormat()), GetWrapModeS(), GetWrapModeT(), IsMipMap(), tlutName);
            } else {
                GXInitTexObj(pTexObj, mImage, mWidth, mHeight, GetTexelFormat(), GetWrapModeS(), GetWrapModeT(), IsMipMap());
            }

            GXInitTexObjLOD(pTexObj, GetMinFilter(), GetMagFilter(), GetMinLOD(), GetMaxLOD(), GetLODBias(), IsBiasClampEnable(), IsEdgeLODEnable(),
=======
                u32 tlutName = J3DNativeTexture::getTlut(pTexObj);
                J3DNativeTexture::initCI(pTexObj, mImage, mWidth, mHeight, GXCITexFmt(GetTexelFormat()), GetWrapModeS(), GetWrapModeT(), IsMipMap(), tlutName);
            } else {
                J3DNativeTexture::init(pTexObj, mImage, mWidth, mHeight, GetTexelFormat(), GetWrapModeS(), GetWrapModeT(), IsMipMap());
            }

            J3DNativeTexture::initLOD(pTexObj, GetMinFilter(), GetMagFilter(), GetMinLOD(), GetMaxLOD(), GetLODBias(), IsBiasClampEnable(), IsEdgeLODEnable(),
>>>>>>> upstream/main
                            GetAnisotropy());
        }

        void TexMap::Get(_GXTlutObj* pTlutObj) const {
<<<<<<< HEAD
            GXInitTlutObj(pTlutObj, GetPalette(), GetPaletteFormat(), GetPaletteEntryNum());
=======
            J3DNativeTexture::initTlut(pTlutObj, GetPalette(), GetPaletteFormat(), GetPaletteEntryNum());
>>>>>>> upstream/main
        }

        void TexMap::Set(const GXTexObj& texObj) {
            void* image;
            u16 width, height;
            GXTexFmt format;
            GXTexWrapMode wrapS, wrapT;
            GXBool mipmap;

<<<<<<< HEAD
            GXGetTexObjAll(&texObj, &image, &width, &height, &format, &wrapS, &wrapT, &mipmap);
=======
            J3DNativeTexture::getAll(&texObj, &image, &width, &height, &format, &wrapS, &wrapT, &mipmap);
>>>>>>> upstream/main

            mImage = image;
            SetSize(width, height);
            mBits.textureFormat = format;
            SetWrapMode(wrapS, wrapT);
            SetMipMap(mipmap);

            GXTexFilter minFilter, magFilter;
            f32 minLOD, maxLOD, lodBias;
            GXBool biasCLampEnable, edgeLODEnable;
            GXAnisotropy aniso;
<<<<<<< HEAD
            GXGetTexObjLODAll(&texObj, &minFilter, &magFilter, &minLOD, &maxLOD, &lodBias, &biasCLampEnable, &edgeLODEnable, &aniso);
=======
            J3DNativeTexture::getLODAll(&texObj, &minFilter, &magFilter, &minLOD, &maxLOD, &lodBias, &biasCLampEnable, &edgeLODEnable, &aniso);
>>>>>>> upstream/main

            SetFilter(minFilter, magFilter);
            SetLOD(minLOD, maxLOD);
            SetLODBias(lodBias);
            SetBiasClampEnable(biasCLampEnable);
            SetEdgeLODEnable(edgeLODEnable);
            mBits.anisotropy = aniso;
        }

<<<<<<< HEAD
        void TexMap::ReplaceImage(const TPLDescriptor* pTPLDesc) {
            const TPLHeader& header = *pTPLDesc->textureHeader;
            mImage = header.data;
            SetSize(header.width, header.height);
            SetTexelFormat(GXTexFmt(u32(header.format)));

            if (const TPLClutHeader* const pClut = pTPLDesc->CLUTHeader) {
                SetPalette(pClut->data);
                SetPaletteFormat(pClut->format);
                SetPaletteEntryNum(pClut->numEntries);
=======
        // A TPL file as the disc has it: big-endian fields, offsets from the file's start. The palette's header holds
        // the version, the image count and the descriptor table; a descriptor names an image header and an optional
        // palette (CLUT) header. Read in place: nothing is relocated.
        void TexMap::ReplaceImage(const void* pTplFile, u32 id) {
            const u8* const file = static_cast< const u8* >(pTplFile);
            const u32 cVersion = 2142000;
            if (nebula_load_be32(file) != cVersion) {
                JUTException::panic_f(__FILE__, __LINE__, "invalid version number for texture palette");
            }
            const u32 count = nebula_load_be32(file + 4);
            const u8* const descriptor = file + nebula_load_be32(file + 8) + (id % count) * 8;

            const u8* const header = file + nebula_load_be32(descriptor);
            const u16 height = nebula_load_be16(header);
            const u16 width = nebula_load_be16(header + 2);
            const u32 format = nebula_load_be32(header + 4);
            void* const image = const_cast< u8* >(file + nebula_load_be32(header + 8));
            const u8 maxLOD = header[34];
            mImage = image;
            J3DNativeTexture::registerImage(image, static_cast< u8 >(format), width, height, static_cast< u8 >(maxLOD + 1));
            SetSize(width, height);
            SetTexelFormat(GXTexFmt(format));

            const u32 clutOffset = nebula_load_be32(descriptor + 4);
            if (clutOffset != 0) {
                const u8* const clut = file + clutOffset;
                SetPalette(const_cast< u8* >(file + nebula_load_be32(clut + 8)));
                SetPaletteFormat(GXTlutFmt(nebula_load_be32(clut + 4)));
                SetPaletteEntryNum(nebula_load_be16(clut));
>>>>>>> upstream/main
            } else {
                SetPalette(nullptr);
                SetPaletteFormat(GXTlutFmt(0));
                SetPaletteEntryNum(0);
            }
        }
<<<<<<< HEAD

        void TexMap::ReplaceImage(TPLPalette* p, u32 id) {
#ifdef TARGET_PC
            // Natively TPLBind itself tells a bound palette from one still in file form: the console's test, a
            // descriptor pointer below MEM1 (an offset), does not hold for host addresses.
            TPLBind(p);
#else
            if (reinterpret_cast< u32 >(p->descriptorArray) < 0x80000000) {
                TPLBind(p);
            }
#endif

            ReplaceImage(TPLGet(p, id));
        }
=======
>>>>>>> upstream/main
    };  // namespace lyt
};  // namespace nw4r
