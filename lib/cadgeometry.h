/*******************************************************************************
 *  Project: libopencad
 *  Purpose: OpenSource CAD formats support library
 *  Author: Alexandr Borzykh, mush3d at gmail.com
 *  Author: Dmitry Baryshnikov, bishop.dev@gmail.com
 *  Language: C++
 *******************************************************************************
 *  The MIT License (MIT)
 *
 *  Copyright (c) 2016 Alexandr Borzykh
 *  Copyright (c) 2016 NextGIS, <info@nextgis.com>
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *******************************************************************************/
#ifndef CADGEOMETRIES_H
#define CADGEOMETRIES_H

#include "cadobjects.h"
#include "cadcolors.h"

#include <array>

using namespace std;

class CADAttdef;

class CADAttrib;

/**
 * @brief The Matrix_CAD class
 */
class Matrix_CAD
{
public:
              Matrix_CAD();
    void      translate( const CADVector& vector );
    void      rotate( double rotation );
    void      scale( const CADVector& vector );
    CADVector multiply( const CADVector& vector ) const;
protected:
    array<double, 9> matrix;
};

/**
 * @brief Base CAD geometry class
 */
class CADGeometry
{
public:
    CADGeometry();
    virtual ~CADGeometry();
    /**
     * @brief The CAD geometry types enum
     */
    enum GeometryType
    {
        UNDEFINED = 0,
        POINT,
        CIRCLE,
        LWPOLYLINE,
        ELLIPSE,
        LINE,
		POLYLINE2D,
        POLYLINE3D,
        TEXT,
        ARC,
        SPLINE,
        SOLID,
        RAY,
        HATCH, // NOT IMPLEMENTED
        IMAGE,
        MTEXT,
        MLINE,
        XLINE,
        FACE3D,
        POLYLINE_PFACE,
        ATTRIB,
        ATTDEF
    };

    enum GeometryType getType() const;
    double            getThickness() const;
    void              setThickness( double thicknes );
    RGBColor          getColor() const;
    void              setColor( RGBColor color );// TODO: in 2004+ ACI is not the only way to set the color.

    vector<CADAttrib> getBlockAttributes() const;
    void              setBlockAttributes( const vector<CADAttrib>& value );

    vector<string> getEED() const;
    void           setEED( vector<string> eed );

    virtual void print() const                     = 0;
    virtual void transform( const Matrix_CAD& matrix ) = 0;
protected:
    vector<CADAttrib> blockAttributes; // attributes of block reference this geometry is attached to.

    vector<string>    asEED;
    enum GeometryType geometryType;
    double            thickness;
    RGBColor          geometry_color;
};

/**
 * @brief Geometry class which represents Unhandled geometry (means that library can't read it yet)
 */
class CADUnknown : public CADGeometry
{
public:
    CADUnknown();

    virtual void print() const override;
    void         transform( const Matrix_CAD& matrix ) override;
};

/**
 * @brief Geometry class which a single Point
 */
class CADPoint3D : public CADGeometry
{
public:
              CADPoint3D();
              CADPoint3D( const CADVector& positionIn, double thicknessIn );
    CADVector getPosition() const;
    void      setPosition( const CADVector& value );

    CADVector getExtrusion() const;
    void      setExtrusion( const CADVector& value );

    double getXAxisAng() const;
    void   setXAxisAng( double value );

    virtual void print() const override;
    virtual void transform( const Matrix_CAD& matrix ) override;
protected:
    CADVector position;
    CADVector extrusion;
    double    xAxisAng;
};

/**
 * @brief Geometry class which represents a simple Line
 */
class CADLine : public CADGeometry
{
public:
    CADLine();
    CADLine( const CADPoint3D& startIn, const CADPoint3D& endIn );

    CADPoint3D getStart() const;
    void       setStart( const CADPoint3D& value );

    CADPoint3D getEnd() const;
    void       setEnd( const CADPoint3D& value );

    virtual void print() const override;
    virtual void transform( const Matrix_CAD& matrix ) override;
protected:
    CADPoint3D start;
    CADPoint3D end;
};

/**
* @brief Geometry class which represents Polyline 2D
*/
class CADPolyline2D : public CADGeometry
{
public:
	CADPolyline2D();

	void	   addVertex(const CADVector& vertex);
	size_t	   getVertexCount() const;
	CADVector& getVertex(size_t index);

	bool isClosed() const;
	void setClosed(bool state);

	bool isSplined() const; // if splined there will be extra 2 vertexes at end defining end vector, if closed two at start also
	void setSplined( bool state );

	double getStartSegWidth() const;
	void   setStartSegWidth( double value );

	double getEndSegWidth() const;
	void   setEndSegWidth( double value );

	double getElevation() const;
	void   setElevation( double value );

	CADVector getVectExtrusion() const;
	void      setVectExtrusion( const CADVector& value );

	vector<pair<double, double> > getWidths() const;
	void                          setWidths(const vector<pair<double, double> >& value);

	bool		   hasBulges() const; // true if any vertexes have non zero bulges
	vector<double> getBulges() const;
	void           setBulges(const vector<double>& value);

	virtual void print() const override;
	virtual void transform( const Matrix_CAD& matrix ) override;

protected:
	bool						  bClosed;
	bool						  bSplined;
	double						  dfStartWidth;
	double						  dfEndWidth;
	double						  elevation;
	CADVector					  vectExtrusion;
	bool						  hasNonZeroBulges;
	vector<double>				  bulges;
	vector<pair<double, double> > widths; // start, end.
	vector<CADVector>	          vertexes;
};


/**
 * @brief Geometry class which represents Polyline 3D
 */
class CADPolyline3D : public CADGeometry
{
public:
    CADPolyline3D();

    void       addVertex( const CADVector& vertex );
    size_t	   getVertexCount() const;
    CADVector& getVertex( size_t index );

	bool isClosed() const;
	void setClosed( bool state );

	bool isSplined() const; // if splined there will be extra 2 vertexes at end defining end direction, if closed two at start also
	void setSplined( bool state );

    virtual void print() const override;
    virtual void transform( const Matrix_CAD& matrix ) override;

protected:
	bool			  bClosed;
	bool			  bSplined;
    vector<CADVector> vertexes;
};

/**
 * @brief Geometry class which represents LWPolyline
 */

class CADLWPolyline : public CADGeometry
{
public:
    CADLWPolyline();

	void       addVertex(const CADVector& vertex);
	size_t	   getVertexCount() const;
	CADVector& getVertex(size_t index);
	const CADVector& getVertex(size_t index) const;

	bool isClosed() const;
	void setClosed(bool state);

    double getConstWidth() const;
    void   setConstWidth( double value );

    double getElevation() const;
    void   setElevation( double value );

    CADVector getVectExtrusion() const;
    void      setVectExtrusion( const CADVector& value );

    vector<pair<double, double> > getWidths() const;
    void                          setWidths( const vector<pair<double, double> >& value );

	bool		   hasBulges() const; // true if any vertexes have non zero bulges
    vector<double> getBulges() const;
    void           setBulges( const vector<double>& value );

    virtual void print() const override;
	virtual void transform(const Matrix_CAD& matrix) override;

protected:
	bool						  bClosed;
    double                        constWidth;
    double                        elevation;
    CADVector                     vectExtrusion;
	bool						  hasNonZeroBulges;
    vector<double>                bulges;
    vector<pair<double, double> > widths; // start, end.
	vector<CADVector>		      vertexes;
};

/**
 * @brief Geometry class which represents Circle
 */
class CADCircle : public CADPoint3D
{
public:
    CADCircle();

    double getRadius() const;
    void   setRadius( double value );

    virtual void print() const override;
protected:
    double radius;
};

/**
 * @brief Geometry class which represents Text
 */
class CADText : public CADPoint3D
{
public:
    CADText();

    string getTextValue() const;
    void   setTextValue( const string& value );

    double getHeight() const;
    void   setHeight( double value );

    double getRotationAngle() const;
    void   setRotationAngle( double value );

    double getObliqueAngle() const;
    void   setObliqueAngle( double value );

    virtual void print() const override;
protected:
    double obliqueAngle;
    double rotationAngle;
    double height;
    string textValue;
};

/**
 * @brief Geometry class which represents Arc
 */
class CADArc : public CADCircle
{
public:
    CADArc();

    double getStartingAngle() const;
    void   setStartingAngle( double value );

    double getEndingAngle() const;
    void   setEndingAngle( double value );

    virtual void print() const override;
protected:
    double startingAngle;
    double endingAngle;
};

/**
 * @brief Geometry class which represents Ellipse
 */
class CADEllipse : public CADArc
{
public:
    CADEllipse();

    double getAxisRatio() const;
    void   setAxisRatio( double value );

    CADVector getSMAxis() const;
    void      setSMAxis( const CADVector& vectSMA );

    virtual void print() const override;
protected:
    CADVector vectSMAxis;
    double    axisRatio;
};

/**
 * @brief Geometry class which represents Spline
 */
class CADSpline : public CADGeometry
{
public:
    CADSpline();

    long getScenario() const;
    void setScenario( long value );

    bool isRational() const;
    void setRational( bool value );

    bool isClosed() const;
    void setClosed( bool value );

    vector<CADVector>& getControlPoints();
    vector<CADVector>& getFitPoints();
    vector<double>   & getControlPointsWeights();

    void addControlPointsWeight( double p_weight );
    void addControlPoint( const CADVector& point );
    void addFitPoint( const CADVector& point );

    bool getWeight() const;
    void setWeight( bool value );

    double getFitTollerance() const;
    void   setFitTollerance( double value );

    long getDegree() const;
    void setDegree( long value );

    virtual void print() const override;
    virtual void transform( const Matrix_CAD& matrix ) override;
protected:
    long   scenario;
    bool   rational;
    bool   closed;
    bool   weight;
    double fitTollerance;
    long   degree;

    vector<double>    ctrlPointsWeight;
    vector<CADVector> avertCtrlPoints;
    vector<CADVector> averFitPoints;
};

/**
 * @brief Geometry class which represents Solid
 */
class CADSolid : public CADPoint3D
{
public:
    CADSolid();

    double getElevation() const;
    void   setElevation( double value );
    void   addCorner( const CADVector& corner );
    vector<CADVector> getCorners();

    virtual void print() const override;
    virtual void transform( const Matrix_CAD& matrix ) override;
protected:
    double            elevation;
    vector<CADVector> avertCorners;
};

/**
 * @brief Geometry class which represents Ray
 */
class CADRay : public CADPoint3D
{
public:
    CADRay();

    CADVector getVectVector() const;
    void      setVectVector( const CADVector& value );

    virtual void print() const override;
};

/**
 * @brief Geometry class which represents Hatch
 */
class CADHatch : public CADGeometry
{
public:
    CADHatch();
};

/**
 * @brief Geometry class which represents Image (Raster Image)
 */
class CADImage : public CADGeometry
{
public:
    /**
     * @brief enum which describes in which units Image resolutions is present 
     */
    enum ResolutionUnit
    {
        NONE = 0, CENTIMETER = 2, INCH = 5
    };

    CADImage();

    CADVector getVertInsertionPoint() const;
    void      setVertInsertionPoint( const CADVector& value );

    CADVector getImageSize() const;
    void      setImageSize( const CADVector& value );

    CADVector getImageSizeInPx() const;
    void      setImageSizeInPx( const CADVector& value );

    CADVector getPixelSizeInACADUnits() const;
    void      setPixelSizeInACADUnits( const CADVector& value );

    short getClippingBoundaryType() const;
    void  setClippingBoundaryType( short value );

    enum ResolutionUnit getResolutionUnits() const;
    void                setResolutionUnits( enum ResolutionUnit value );

    string getFilePath() const;
    void   setFilePath( const string& value );

    void setOptions( bool transparency, bool clip, unsigned char brightness, unsigned char contrast );

    void addClippingPoint( const CADVector& pt );

    virtual void print() const override;
    virtual void transform( const Matrix_CAD& matrix ) override;
protected:
    CADVector     vertInsertionPoint;
    //CADVector vectUDirection;
    //CADVector vectVDirection;
    CADVector     imageSize;
    //bool bShow;
    //bool bShowWhenNotAlignedWithScreen;
    //bool bUseClippingBoundary;
    bool          bTransparency;
    bool          bClipping;
    unsigned char dBrightness;
    unsigned char dContrast;
    //char dFade;

    CADVector           imageSizeInPx;
    string              filePath;
    //bool bIsLoaded;
    enum ResolutionUnit resolutionUnits;
    //unsigned char       resolutionUnit; // 0 == none, 2 == centimeters, 5 == inches;
    CADVector           pixelSizeInACADUnits;

    short             clippingBoundaryType; // 1 == rect, 2 == polygon
    vector<CADVector> avertClippingPolygon;
};

/**
 * @brief Geometry class which represents MText
 */
class CADMText : public CADText
{
public:
    CADMText();

    double getRectWidth() const;
    void   setRectWidth( double value );

    double getExtents() const;
    void   setExtents( double value );

    double getExtentsWidth() const;
    void   setExtentsWidth( double value );

    virtual void print() const override;
protected:
    double rectWidth;
    double extents;
    double extentsWidth;
    // TODO: do we need this here?
    //short dDrawingDir;
    //short dLineSpacingStyle;
    //short dLineSpacingFactor;
    //long dBackgroundFlags; // R2004+
    //long dBackgroundScaleFactor;
    //short dBackgroundColor;
    //long dBackgroundTransparency;
};

/**
 * @brief Geometry class which represents 3DFace
 */
class CADFace3D : public CADGeometry
{
public:
    CADFace3D();

    void      addCorner( const CADVector& corner );
    CADVector getCorner( size_t index );

    short getInvisFlags() const;
    void  setInvisFlags( short value );

    virtual void print() const override;
    virtual void transform( const Matrix_CAD& matrix ) override;
protected:
    vector<CADVector> avertCorners;
    short             invisFlags;
};

/**
 * @brief Geometry class which represents Polyline (PFace)
 */
class CADPolylinePFace : public CADGeometry
{
public:
    CADPolylinePFace();

    void addVertex( const CADVector& vertex );

    virtual void print() const override;
    virtual void transform( const Matrix_CAD& matrix ) override;
protected:
    vector<CADVector> vertexes;
};

/**
 * @brief Geometry class which represents XLine
 */
class CADXLine : public CADRay
{
public:
    CADXLine();

    virtual void print() const override;
};

/**
 * @brief Geometry class which represents MLine
 */
class CADMLine : public CADPoint3D
{
public:
    CADMLine();

    double getScale() const;
    void   setScale( double value );

    bool isOpened() const;
    void setOpened( bool value );

    void addVertex( const CADVector& vertex );

    virtual void print() const override;
    virtual void transform( const Matrix_CAD& matrix ) override;
protected:
    double            scale;
    //char dJust;
    bool              opened; // 1 == open, 0 == close
    // TODO: do we need more properties here?
    vector<CADVector> avertVertexes;
};

/**
 * @brief Geometry class which represents Attribute
 */
class CADAttrib : public CADText
{
public:
    CADAttrib();

    double getElevation() const;
    void   setElevation( double );

    string getTag() const;
    void   setTag( const string& );

    CADVector getAlignmentPoint() const;
    void      setAlignmentPoint( const CADVector& );

    bool isPositionLocked() const;
    void setPositionLocked( bool );

    virtual void print() const override;
    virtual void transform( const Matrix_CAD& matrix ) override;
protected:
    CADVector vertAlignmentPoint;
    double    dfElevation;
    string    sTag;
    bool      bLockPosition;
};

/**
 * @brief Geometry class which represents Attribute definition
 */
class CADAttdef : public CADAttrib
{
public:
    CADAttdef();

    string getPrompt() const;
    void   setPrompt( const string& );

    virtual void print() const override;
protected:
    string sPrompt;
};

//class EXTERN LineType
//{
//public:
//    string sEntryName;
//    string sDescription;
//    double dfPatternLen;
//    char dAlignment;
//    char nNumDashes;
//    struct Dash
//    {
//        double dfLength;
//        short dComplexShapecode;
//        double dfXOffset;
//        double dfYOffset;
//        double dfScale;
//        double dfRotation;
//        short dShapeflag;
//    };
//    vector < char > abyTextArea; // TODO: what is it?
//    vector < CADHandle > hShapefiles; // TODO: one for each dash?
//};

//class EXTERN Block
//{
//public:
//    Block(CADFile * pCADFile)
//    {
//        pstCADFile_m = pCADFile;
//    }
//
//    string sBlockName;
//
//    CADFile * pstCADFile_m;
//
//    vector < pair < long long, short > > astAttachedGeometries;
//};


#endif // CADGEOMETRIES_H
