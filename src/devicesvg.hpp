/* *************************************************************************
                          devicesvg.hpp  -  SVG device
                             -------------------
    begin                : December 26 2008
    copyright            : (C) 2008 by Sylwester Arabas
    email                : slayoo@users.sf.net
 ***************************************************************************/

/* *************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DEVICESVG_HPP_
#define DEVICESVG_HPP_

#include "gdlsvgstream.hpp"

static const float SVG_CM2IN = (1.0 / INCHToCM) ;
static const float SVG_RESOL = 1000.0; // per cm
static const PLFLT SVG_DPI = 72; 

class DeviceSVG : public GraphicsDevice
{
  std::string      fileName;
  GDLSVGStream*    actStream;
  float            XPageSize;
  float            YPageSize;
  float            XOffset;
  float            YOffset;
  int              color;
  int              decomposed; // false -> use color table
  bool	           orient_portrait; 
  float	           scale;
  
  void InitStream()
  {
    delete actStream;

    DLongGDL* pMulti = SysVar::GetPMulti();
    DLong nx = (*pMulti)[ 1];
    DLong ny = (*pMulti)[ 2];

    if( nx <= 0) nx = 1;
    if( ny <= 0) ny = 1;
    actStream = new GDLSVGStream( nx, ny);

    actStream->sfnam( fileName.c_str());

    actStream->spage(SVG_DPI, SVG_DPI, XPageSize*72*SVG_CM2IN, YPageSize*72*SVG_CM2IN, XOffset*72*SVG_CM2IN, YOffset*72*SVG_CM2IN);

    // no pause on destruction
    actStream->spause( false);

    // extended fonts
    actStream->fontld( 1);

    PLINT r[ctSize], g[ctSize], b[ctSize];
    actCT.Get( r, g, b);
    actStream->SetColorMap0( r, g, b, ctSize);
    actStream->SetColorMap1( r, g, b, ctSize);
    actStream->scolbg(255,255,255); // start with a white background

    actStream->Init();

    // need to be called initially. permit to fix things
    actStream->plstream::ssub(1, 1); // plstream below stays with ONLY ONE page
    actStream->plstream::adv(0); //-->this one is the 1st and only pladv
    // load font
    actStream->plstream::font(1);
    actStream->plstream::vpor(0, 1, 0, 1);
    actStream->plstream::wind(0, 1, 0, 1);

    actStream->ssub(1, 1);
    actStream->adv(0); //this is for us (counters)
    actStream->SetPageDPMM();
    actStream->DefaultCharSize();
  }

public:
  DeviceSVG(): GraphicsDevice(), fileName( "gdl.svg"), actStream( NULL),
	       XPageSize(29.7), YPageSize(21.0), XOffset(0.0), YOffset(0.0),
    color(1),  decomposed(1), scale(1.)
  {
    name = "SVG";

    DLongGDL origin( dimension( 2));
    DLongGDL zoom( dimension( 2));
    zoom[0] = 1;
    zoom[1] = 1;

    dStruct = new DStructGDL( "!DEVICE");
    dStruct->InitTag("NAME",       DStringGDL( name)); 
    dStruct->InitTag("X_SIZE",     DLongGDL( XPageSize*scale*SVG_RESOL)); 
    dStruct->InitTag("Y_SIZE",     DLongGDL( YPageSize*scale*SVG_RESOL)); 
    dStruct->InitTag("X_VSIZE",    DLongGDL( XPageSize*scale*SVG_RESOL)); 
    dStruct->InitTag("Y_VSIZE",    DLongGDL( YPageSize*scale*SVG_RESOL)); 
    dStruct->InitTag("X_CH_SIZE",  DLongGDL( 0.25*scale*SVG_RESOL)); 
    dStruct->InitTag("Y_CH_SIZE",  DLongGDL( 1.5*0.25*scale*SVG_RESOL)); 
    dStruct->InitTag("X_PX_CM",    DFloatGDL( SVG_RESOL)); 
    dStruct->InitTag("Y_PX_CM",    DFloatGDL( SVG_RESOL)); 
    dStruct->InitTag("N_COLORS",   DLongGDL( 16777216)); 
    dStruct->InitTag("TABLE_SIZE", DLongGDL( 256)); 
    dStruct->InitTag("FILL_DIST",  DLongGDL( 0)); 
    dStruct->InitTag("WINDOW",     DLongGDL( -1)); 
    dStruct->InitTag("UNIT",       DLongGDL( 0)); 
    dStruct->InitTag("FLAGS",      DLongGDL( 266807)); //if 266295 (i.e., not a "PRINTER") ERASE would work on it, but it is probably not what we want. 
    dStruct->InitTag("ORIGIN",     origin); 
    dStruct->InitTag("ZOOM",       zoom); 
    
    SetLandscape();
  }
  
  ~DeviceSVG()
  {
    delete actStream;
  }

  GDLGStream* GetStream( bool open=true)
  {
    if( actStream == NULL) 
      {
	if( !open) return NULL;
	InitStream();
      }
    return actStream;
  }

  bool SetFileName( const std::string& f)
  {
    fileName = f;
    return true;
  }

  bool CloseFile()
  {
    delete actStream;
    actStream = NULL;
    return true;
  }
  
  bool SetXOffset( const float xo) // xo [cm]
  {
    // nothing for the moment (coordinates tricky)
    //    XOffset=xo;
    return true;
  }

  bool SetYOffset( const float yo) // yo [cm]
  {
    // nothing for the moment (coordinates tricky)
    // YOffset=yo;
    return true;
  }

  bool SetXPageSize( const float xs) // xs [cm]
  {
    // nothing for the moment (coordinates tricky)
    XPageSize=xs;
    (*static_cast<DLongGDL*>(dStruct->GetTag(dStruct->Desc()->TagIndex("X_SIZE"))))[0] 
      = DLong(floor(0.5+
        xs * (*static_cast<DFloatGDL*>(dStruct->GetTag(dStruct->Desc()->TagIndex("X_PX_CM"))))[0]
      ));
   (*static_cast<DLongGDL*>(dStruct->GetTag(dStruct->Desc()->TagIndex("X_VSIZE"))))[0]
      = DLong(floor(0.5+
        xs * (*static_cast<DFloatGDL*>(dStruct->GetTag(dStruct->Desc()->TagIndex("X_PX_CM"))))[0]
      ));
    return true;
  }

  bool SetYPageSize( const float ys) // ys [cm]
  {
    // nothing for the moment (coordinates tricky)
    YPageSize=ys;
    (*static_cast<DLongGDL*>(dStruct->GetTag(dStruct->Desc()->TagIndex("Y_SIZE"))))[0] 
      = DLong(floor(0.5+
        ys * (*static_cast<DFloatGDL*>(dStruct->GetTag(dStruct->Desc()->TagIndex("Y_PX_CM"))))[0]
      ));
    (*static_cast<DLongGDL*>(dStruct->GetTag(dStruct->Desc()->TagIndex("Y_VSIZE"))))[0]
      = DLong(floor(0.5+
        ys * (*static_cast<DFloatGDL*>(dStruct->GetTag(dStruct->Desc()->TagIndex("Y_PX_CM"))))[0]
      ));
    return true;
  }

  bool SetColor(const long hascolor)
  {
    if (hascolor==1) color=1; else color=0;
      if (hascolor==1) 
      {
        DLong FLAG=(*static_cast<DLongGDL*>( dStruct->GetTag(dStruct->Desc()->TagIndex("FLAGS"))))[0];
        (*static_cast<DLongGDL*>( dStruct->GetTag(dStruct->Desc()->TagIndex("FLAGS"))))[0]=FLAG|16; //set colored device
      } else {
      DLong FLAG=(*static_cast<DLongGDL*>( dStruct->GetTag(dStruct->Desc()->TagIndex("FLAGS"))))[0];
        (*static_cast<DLongGDL*>( dStruct->GetTag(dStruct->Desc()->TagIndex("FLAGS"))))[0]=FLAG&(~16); //set monochrome device
      }
    return true;
  }

  bool SetPortrait()
  {
    // nothing for the moment (coordinates tricky)
    //    orient_portrait = true;
    return true;
  }

  bool SetLandscape()
  {
    // nothing for the moment (coordinates tricky)
    //    orient_portrait = false;
    return true;
  }

  bool SetScale(float value)
  {
    //    scale = value;
    return true;
  }
  
  bool Decomposed( bool value)           
  {   
    decomposed = value;
    if (decomposed) (*static_cast<DLongGDL*>( dStruct->GetTag(dStruct->Desc()->TagIndex("N_COLORS"))))[0]=256*256*256;
    else (*static_cast<DLongGDL*>( dStruct->GetTag(dStruct->Desc()->TagIndex("N_COLORS"))))[0]=256;
    return true;
  }

  DLong GetDecomposed()        
  {
    return decomposed;  
  }
  
  DIntGDL* GetPageSize()
  {
    DIntGDL* res;
    res = new DIntGDL(2, BaseGDL::NOZERO);
    (*res)[0]= XPageSize*SVG_DPI*SVG_CM2IN;
    (*res)[1]= YPageSize*SVG_DPI*SVG_CM2IN;
    return res;
  }
};

#endif
