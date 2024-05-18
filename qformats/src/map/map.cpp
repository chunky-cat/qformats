#include "map.h"
#include "qmath.h"
#include <iostream>

namespace qformats::map
{
    void QMap::LoadFile(const std::string &filename)
    {
        auto qfile = new QMapFile();
        qfile->Parse(filename);
        map_file = qfile;
    }

    void QMap::GenerateGeometry()
    {
        for (const auto &e : map_file->brushEntities)
        {
            SolidEntity se;
            se.entityRef = e;
            for (auto &b : e->brushes)
            {
                b.buildGeometry();
                se.geoBrushes.push_back(b);
            }
            solidEntities.push_back(se);
        }
        // TODO: does not work at all
        // MergeBrushes();
    }

    void QMap::LoadTextures(textures::textureRequestCb cb)
    {
        texMan.OnTextureRequested(cb);

        for (auto t : map_file->textures)
        {
            texMan.GetOrAddTexture(t);
        }

        for (auto &se : solidEntities)
            for (auto &b : se.geoBrushes)
            {
                for (auto p : b.polygons)
                {
                    auto tex = texMan.GetTexture(p->faceRef.textureID);
                    if (tex == nullptr)
                    {
                        continue;
                    }

                    for (auto &v : p->vertices)
                    {
                        v.uv = QMath::CalcUV(b.hasValveUV, v.point, p->faceRef, tex->Width(), tex->Height());
                    }
                }
            }
    }

    void QMap::MergeBrushes()
    {
        for (auto &se : solidEntities)
        {
            std::vector<Brush> clippedBrushes = se.geoBrushes;
            for (int i = 0; i < se.geoBrushes.size(); i++)
            {
                auto currBrush = clippedBrushes[i];
                bool clipOnPlane = false;
                int j = 0;
                for (auto it = se.geoBrushes.begin(); it != se.geoBrushes.end(); it++, j++)
                {
                    if (i == j)
                    {
                        clipOnPlane = true;
                    }
                    else
                    {
                        if (currBrush.DoesIntersect(*it))
                        {
                            // TODO
                            std::cout << i << " intersects with " << j << std::endl;
                            se.geoBrushes.erase(it);
                            it = se.geoBrushes.begin();
                            j = 0;
                        }
                    }
                }
            }
        }
    }

    /*

QMap::MergeList ( )
{
    Brush			*pClippedList	= CopyList ( );
    Brush			*pClip			= pClippedList;
    Brush			*pBrush			= NULL;
    Poly			*pPolyList		= NULL;

    bool			bClipOnPlane	= false;
    unsigned int	uiBrushes		= GetNumberOfBrushes ( );

    for ( int i = 0; i < uiBrushes; i++ )
    {
        pBrush			= this;
        bClipOnPlane	= false;

        for ( int j = 0; j < uiBrushes; j++ )
        {
            if ( i == j )
            {
                bClipOnPlane = true;
            }
            else
            {
                if ( pClip->AABBIntersect ( pBrush ) )
                {
                    pClip->ClipToBrush ( pBrush, bClipOnPlane );
                }
            }

            pBrush = pBrush->GetNext ( );
        }

        pClip = pClip->GetNext ( );
    }

    pClip = pClippedList;

    while ( pClip != NULL )
    {
        if ( pClip->GetNumberOfPolys ( ) != 0 )
        {
            //
            // Extract brushes left over polygons and add them to the list
            //
            Poly *pPoly = pClip->GetPolys ( )->CopyList ( );

            if ( pPolyList == NULL )
            {
                pPolyList = pPoly;
            }
            else
            {
                pPolyList->AddPoly ( pPoly );
            }

            pClip = pClip->GetNext ( );
        }
        else
        {
            //
            // Brush has no polygons and should be deleted
            //
            if ( pClip == pClippedList )
            {
                pClip = pClippedList->GetNext ( );

                pClippedList->SetNext ( NULL );

                delete pClippedList;

                pClippedList = pClip;
            }
            else
            {
                Brush	*pTemp = pClippedList;

                while ( pTemp != NULL )
                {
                    if ( pTemp->GetNext ( ) == pClip )
                    {
                        break;
                    }

                    pTemp = pTemp->GetNext ( );
                }

                pTemp->m_pNext = pClip->GetNext ( );
                pClip->SetNext ( NULL );

                delete pClip;

                pClip = pTemp->GetNext ( );
            }
        }
    }

    delete pClippedList;

    return pPolyList;
}

*/
}
