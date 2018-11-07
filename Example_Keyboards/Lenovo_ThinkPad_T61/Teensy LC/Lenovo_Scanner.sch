<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eagle SYSTEM "eagle.dtd">
<eagle version="8.2.2">
<drawing>
<settings>
<setting alwaysvectorfont="yes"/>
<setting verticaltext="up"/>
</settings>
<grid distance="0.1" unitdist="inch" unit="inch" style="lines" multiple="1" display="no" altdistance="0.01" altunitdist="inch" altunit="inch"/>
<layers>
<layer number="1" name="Top" color="4" fill="1" visible="no" active="no"/>
<layer number="16" name="Bottom" color="1" fill="1" visible="no" active="no"/>
<layer number="17" name="Pads" color="2" fill="1" visible="no" active="no"/>
<layer number="18" name="Vias" color="2" fill="1" visible="no" active="no"/>
<layer number="19" name="Unrouted" color="6" fill="1" visible="no" active="no"/>
<layer number="20" name="Dimension" color="15" fill="1" visible="no" active="no"/>
<layer number="21" name="tPlace" color="16" fill="1" visible="no" active="no"/>
<layer number="22" name="bPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="23" name="tOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="24" name="bOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="25" name="tNames" color="7" fill="1" visible="no" active="no"/>
<layer number="26" name="bNames" color="7" fill="1" visible="no" active="no"/>
<layer number="27" name="tValues" color="7" fill="1" visible="no" active="no"/>
<layer number="28" name="bValues" color="7" fill="1" visible="no" active="no"/>
<layer number="29" name="tStop" color="7" fill="3" visible="no" active="no"/>
<layer number="30" name="bStop" color="7" fill="6" visible="no" active="no"/>
<layer number="31" name="tCream" color="7" fill="4" visible="no" active="no"/>
<layer number="32" name="bCream" color="7" fill="5" visible="no" active="no"/>
<layer number="33" name="tFinish" color="6" fill="3" visible="no" active="no"/>
<layer number="34" name="bFinish" color="6" fill="6" visible="no" active="no"/>
<layer number="35" name="tGlue" color="7" fill="4" visible="no" active="no"/>
<layer number="36" name="bGlue" color="7" fill="5" visible="no" active="no"/>
<layer number="37" name="tTest" color="7" fill="1" visible="no" active="no"/>
<layer number="38" name="bTest" color="7" fill="1" visible="no" active="no"/>
<layer number="39" name="tKeepout" color="4" fill="11" visible="no" active="no"/>
<layer number="40" name="bKeepout" color="1" fill="11" visible="no" active="no"/>
<layer number="41" name="tRestrict" color="4" fill="10" visible="no" active="no"/>
<layer number="42" name="bRestrict" color="1" fill="10" visible="no" active="no"/>
<layer number="43" name="vRestrict" color="2" fill="10" visible="no" active="no"/>
<layer number="44" name="Drills" color="7" fill="1" visible="no" active="no"/>
<layer number="45" name="Holes" color="7" fill="1" visible="no" active="no"/>
<layer number="46" name="Milling" color="3" fill="1" visible="no" active="no"/>
<layer number="47" name="Measures" color="7" fill="1" visible="no" active="no"/>
<layer number="48" name="Document" color="7" fill="1" visible="no" active="no"/>
<layer number="49" name="Reference" color="7" fill="1" visible="no" active="no"/>
<layer number="50" name="dxf" color="7" fill="1" visible="no" active="no"/>
<layer number="51" name="tDocu" color="14" fill="1" visible="no" active="no"/>
<layer number="52" name="bDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="53" name="tGND_GNDA" color="7" fill="9" visible="no" active="no"/>
<layer number="54" name="bGND_GNDA" color="1" fill="9" visible="no" active="no"/>
<layer number="56" name="wert" color="7" fill="1" visible="no" active="no"/>
<layer number="57" name="tCAD" color="7" fill="1" visible="no" active="no"/>
<layer number="59" name="tCarbon" color="7" fill="1" visible="no" active="no"/>
<layer number="60" name="bCarbon" color="7" fill="1" visible="no" active="no"/>
<layer number="90" name="Modules" color="5" fill="1" visible="yes" active="yes"/>
<layer number="91" name="Nets" color="2" fill="1" visible="yes" active="yes"/>
<layer number="92" name="Busses" color="1" fill="1" visible="yes" active="yes"/>
<layer number="93" name="Pins" color="2" fill="1" visible="no" active="yes"/>
<layer number="94" name="Symbols" color="4" fill="1" visible="yes" active="yes"/>
<layer number="95" name="Names" color="7" fill="1" visible="yes" active="yes"/>
<layer number="96" name="Values" color="7" fill="1" visible="yes" active="yes"/>
<layer number="97" name="Info" color="7" fill="1" visible="yes" active="yes"/>
<layer number="98" name="Guide" color="6" fill="1" visible="yes" active="yes"/>
<layer number="99" name="SpiceOrder" color="7" fill="1" visible="yes" active="yes"/>
<layer number="100" name="Muster" color="7" fill="1" visible="no" active="no"/>
<layer number="101" name="Patch_Top" color="12" fill="4" visible="yes" active="yes"/>
<layer number="102" name="Vscore" color="7" fill="1" visible="yes" active="yes"/>
<layer number="103" name="tMap" color="7" fill="1" visible="yes" active="yes"/>
<layer number="104" name="Name" color="16" fill="1" visible="yes" active="yes"/>
<layer number="105" name="tPlate" color="7" fill="1" visible="yes" active="yes"/>
<layer number="106" name="bPlate" color="7" fill="1" visible="yes" active="yes"/>
<layer number="107" name="Crop" color="7" fill="1" visible="yes" active="yes"/>
<layer number="108" name="tplace-old" color="10" fill="1" visible="yes" active="yes"/>
<layer number="109" name="ref-old" color="11" fill="1" visible="yes" active="yes"/>
<layer number="110" name="fp0" color="7" fill="1" visible="yes" active="yes"/>
<layer number="111" name="LPC17xx" color="7" fill="1" visible="yes" active="yes"/>
<layer number="112" name="tSilk" color="7" fill="1" visible="yes" active="yes"/>
<layer number="113" name="IDFDebug" color="4" fill="1" visible="yes" active="yes"/>
<layer number="114" name="Badge_Outline" color="7" fill="1" visible="yes" active="yes"/>
<layer number="115" name="ReferenceISLANDS" color="7" fill="1" visible="yes" active="yes"/>
<layer number="116" name="Patch_BOT" color="9" fill="4" visible="yes" active="yes"/>
<layer number="118" name="Rect_Pads" color="7" fill="1" visible="yes" active="yes"/>
<layer number="121" name="_tsilk" color="7" fill="1" visible="yes" active="yes"/>
<layer number="122" name="_bsilk" color="7" fill="1" visible="yes" active="yes"/>
<layer number="123" name="tTestmark" color="7" fill="1" visible="yes" active="yes"/>
<layer number="124" name="bTestmark" color="7" fill="1" visible="yes" active="yes"/>
<layer number="125" name="_tNames" color="7" fill="1" visible="yes" active="yes"/>
<layer number="126" name="_bNames" color="7" fill="1" visible="yes" active="yes"/>
<layer number="127" name="_tValues" color="7" fill="1" visible="yes" active="yes"/>
<layer number="128" name="_bValues" color="7" fill="1" visible="yes" active="yes"/>
<layer number="129" name="Mask" color="7" fill="1" visible="yes" active="yes"/>
<layer number="131" name="tAdjust" color="7" fill="1" visible="yes" active="yes"/>
<layer number="132" name="bAdjust" color="7" fill="1" visible="yes" active="yes"/>
<layer number="144" name="Drill_legend" color="7" fill="1" visible="yes" active="yes"/>
<layer number="150" name="Notes" color="7" fill="1" visible="yes" active="yes"/>
<layer number="151" name="HeatSink" color="7" fill="1" visible="yes" active="yes"/>
<layer number="152" name="_bDocu" color="7" fill="1" visible="yes" active="yes"/>
<layer number="153" name="FabDoc1" color="7" fill="1" visible="yes" active="yes"/>
<layer number="154" name="FabDoc2" color="7" fill="1" visible="yes" active="yes"/>
<layer number="155" name="FabDoc3" color="7" fill="1" visible="yes" active="yes"/>
<layer number="199" name="Contour" color="7" fill="1" visible="yes" active="yes"/>
<layer number="200" name="200bmp" color="1" fill="10" visible="yes" active="yes"/>
<layer number="201" name="201bmp" color="2" fill="10" visible="yes" active="yes"/>
<layer number="202" name="202bmp" color="3" fill="10" visible="yes" active="yes"/>
<layer number="203" name="203bmp" color="4" fill="10" visible="yes" active="yes"/>
<layer number="204" name="204bmp" color="5" fill="10" visible="yes" active="yes"/>
<layer number="205" name="205bmp" color="6" fill="10" visible="yes" active="yes"/>
<layer number="206" name="206bmp" color="7" fill="10" visible="yes" active="yes"/>
<layer number="207" name="207bmp" color="8" fill="10" visible="yes" active="yes"/>
<layer number="208" name="208bmp" color="9" fill="10" visible="yes" active="yes"/>
<layer number="209" name="209bmp" color="7" fill="1" visible="yes" active="yes"/>
<layer number="210" name="210bmp" color="7" fill="1" visible="yes" active="yes"/>
<layer number="211" name="211bmp" color="7" fill="1" visible="yes" active="yes"/>
<layer number="212" name="212bmp" color="7" fill="1" visible="yes" active="yes"/>
<layer number="213" name="213bmp" color="7" fill="1" visible="yes" active="yes"/>
<layer number="214" name="214bmp" color="7" fill="1" visible="yes" active="yes"/>
<layer number="215" name="215bmp" color="7" fill="1" visible="yes" active="yes"/>
<layer number="216" name="216bmp" color="7" fill="1" visible="yes" active="yes"/>
<layer number="217" name="217bmp" color="18" fill="1" visible="no" active="no"/>
<layer number="218" name="218bmp" color="19" fill="1" visible="no" active="no"/>
<layer number="219" name="219bmp" color="20" fill="1" visible="no" active="no"/>
<layer number="220" name="220bmp" color="21" fill="1" visible="no" active="no"/>
<layer number="221" name="221bmp" color="22" fill="1" visible="no" active="no"/>
<layer number="222" name="222bmp" color="23" fill="1" visible="no" active="no"/>
<layer number="223" name="223bmp" color="24" fill="1" visible="no" active="no"/>
<layer number="224" name="224bmp" color="25" fill="1" visible="no" active="no"/>
<layer number="225" name="225bmp" color="7" fill="1" visible="yes" active="yes"/>
<layer number="226" name="226bmp" color="7" fill="1" visible="yes" active="yes"/>
<layer number="227" name="227bmp" color="7" fill="1" visible="yes" active="yes"/>
<layer number="228" name="228bmp" color="7" fill="1" visible="yes" active="yes"/>
<layer number="229" name="229bmp" color="7" fill="1" visible="yes" active="yes"/>
<layer number="230" name="230bmp" color="7" fill="1" visible="yes" active="yes"/>
<layer number="231" name="231bmp" color="7" fill="1" visible="yes" active="yes"/>
<layer number="232" name="Eagle3D_PG2" color="7" fill="1" visible="yes" active="yes"/>
<layer number="233" name="Eagle3D_PG3" color="7" fill="1" visible="yes" active="yes"/>
<layer number="248" name="Housing" color="7" fill="1" visible="yes" active="yes"/>
<layer number="249" name="Edge" color="7" fill="1" visible="yes" active="yes"/>
<layer number="250" name="Descript" color="3" fill="1" visible="no" active="no"/>
<layer number="251" name="SMDround" color="12" fill="11" visible="no" active="no"/>
<layer number="254" name="cooling" color="7" fill="1" visible="yes" active="yes"/>
<layer number="255" name="routoute" color="7" fill="1" visible="yes" active="yes"/>
</layers>
<schematic xreflabel="%F%N/%S.%C%R" xrefpart="/%S.%C%R">
<libraries>
<library name="SparkFun-Connectors">
<description>&lt;h3&gt;SparkFun Connectors&lt;/h3&gt;
This library contains electrically-functional connectors. 
&lt;br&gt;
&lt;br&gt;
We've spent an enormous amount of time creating and checking these footprints and parts, but it is &lt;b&gt; the end user's responsibility&lt;/b&gt; to ensure correctness and suitablity for a given componet or application. 
&lt;br&gt;
&lt;br&gt;If you enjoy using this library, please buy one of our products at &lt;a href=" www.sparkfun.com"&gt;SparkFun.com&lt;/a&gt;.
&lt;br&gt;
&lt;br&gt;
&lt;b&gt;Licensing:&lt;/b&gt; Creative Commons ShareAlike 4.0 International - https://creativecommons.org/licenses/by-sa/4.0/ 
&lt;br&gt;
&lt;br&gt;
You are welcome to use this library for commercial purposes. For attribution, we ask that when you begin to sell your device using our footprint, you email us with a link to the product being sold. We want bragging rights that we helped (in a very small part) to create your 8th world wonder. We would like the opportunity to feature your device on our homepage.</description>
<packages>
<package name="1X01_LONGPAD">
<description>&lt;h3&gt;Plated Through Hole - Long Pad&lt;/h3&gt;
&lt;p&gt;Specifications:
&lt;ul&gt;&lt;li&gt;Pin count:1&lt;/li&gt;
&lt;li&gt;Pin pitch:0.1"&lt;/li&gt;
&lt;/ul&gt;&lt;/p&gt;
&lt;p&gt;Example device(s):
&lt;ul&gt;&lt;li&gt;CONN_01&lt;/li&gt;
&lt;/ul&gt;&lt;/p&gt;</description>
<wire x1="-1.27" y1="0.635" x2="-1.27" y2="-0.635" width="0.2032" layer="21"/>
<wire x1="1.27" y1="0.635" x2="1.27" y2="-0.635" width="0.2032" layer="21"/>
<pad name="1" x="0" y="0" drill="1.1176" diameter="1.8796" shape="long" rot="R90"/>
<text x="-1.27" y="2.032" size="0.6096" layer="25" font="vector" ratio="20">&gt;NAME</text>
<text x="-1.27" y="-2.667" size="0.6096" layer="27" font="vector" ratio="20">&gt;VALUE</text>
<rectangle x1="-0.254" y1="-0.254" x2="0.254" y2="0.254" layer="51"/>
</package>
<package name="1X01">
<description>&lt;h3&gt;Plated Through Hole&lt;/h3&gt;
&lt;p&gt;Specifications:
&lt;ul&gt;&lt;li&gt;Pin count:1&lt;/li&gt;
&lt;li&gt;Pin pitch:0.1"&lt;/li&gt;
&lt;/ul&gt;&lt;/p&gt;
&lt;p&gt;Example device(s):
&lt;ul&gt;&lt;li&gt;CONN_01&lt;/li&gt;
&lt;/ul&gt;&lt;/p&gt;</description>
<wire x1="1.27" y1="0.635" x2="0.635" y2="1.27" width="0.2032" layer="21"/>
<wire x1="0.635" y1="1.27" x2="-0.635" y2="1.27" width="0.2032" layer="21"/>
<wire x1="-0.635" y1="1.27" x2="-1.27" y2="0.635" width="0.2032" layer="21"/>
<wire x1="-1.27" y1="0.635" x2="-1.27" y2="-0.635" width="0.2032" layer="21"/>
<wire x1="-1.27" y1="-0.635" x2="-0.635" y2="-1.27" width="0.2032" layer="21"/>
<wire x1="-0.635" y1="-1.27" x2="0.635" y2="-1.27" width="0.2032" layer="21"/>
<wire x1="0.635" y1="-1.27" x2="1.27" y2="-0.635" width="0.2032" layer="21"/>
<wire x1="1.27" y1="-0.635" x2="1.27" y2="0.635" width="0.2032" layer="21"/>
<pad name="1" x="0" y="0" drill="1.016" diameter="1.8796" rot="R90"/>
<text x="-1.27" y="1.397" size="0.6096" layer="25" font="vector" ratio="20">&gt;NAME</text>
<text x="-1.27" y="-1.524" size="0.6096" layer="27" font="vector" ratio="20" align="top-left">&gt;VALUE</text>
<rectangle x1="-0.254" y1="-0.254" x2="0.254" y2="0.254" layer="51"/>
</package>
<package name="1X01_2MM">
<description>&lt;h3&gt;Plated Through Hole - 2mm&lt;/h3&gt;
&lt;p&gt;Specifications:
&lt;ul&gt;&lt;li&gt;Pin count:1&lt;/li&gt;
&lt;/ul&gt;&lt;/p&gt;
&lt;p&gt;Example device(s):
&lt;ul&gt;&lt;li&gt;CONN_01&lt;/li&gt;
&lt;/ul&gt;&lt;/p&gt;</description>
<pad name="1" x="0" y="0" drill="2" diameter="3.302" rot="R90"/>
<text x="-1.651" y="1.778" size="0.6096" layer="25" font="vector" ratio="20">&gt;NAME</text>
<text x="-1.651" y="-2.413" size="0.6096" layer="27" font="vector" ratio="20">&gt;VALUE</text>
</package>
<package name="1X01_OFFSET">
<description>&lt;h3&gt;Plated Through Hole - Long Pad w/ Offset Hole&lt;/h3&gt;
&lt;p&gt;Specifications:
&lt;ul&gt;&lt;li&gt;Pin count:1&lt;/li&gt;
&lt;/ul&gt;&lt;/p&gt;
&lt;p&gt;Example device(s):
&lt;ul&gt;&lt;li&gt;CONN_01&lt;/li&gt;
&lt;/ul&gt;&lt;/p&gt;</description>
<wire x1="-1.27" y1="0.635" x2="-1.27" y2="-0.635" width="0.2032" layer="21"/>
<wire x1="-1.27" y1="-0.635" x2="-0.635" y2="-1.27" width="0.2032" layer="21"/>
<wire x1="-0.635" y1="-1.27" x2="0.635" y2="-1.27" width="0.2032" layer="21"/>
<wire x1="0.635" y1="-1.27" x2="1.27" y2="-0.635" width="0.2032" layer="21"/>
<wire x1="1.27" y1="-0.635" x2="1.27" y2="0.635" width="0.2032" layer="21"/>
<pad name="1" x="0" y="0" drill="1.1176" diameter="1.8796" shape="offset" rot="R90"/>
<text x="-1.27" y="3.048" size="0.6096" layer="25" font="vector" ratio="20">&gt;NAME</text>
<text x="-1.27" y="-2.032" size="0.6096" layer="27" font="vector" ratio="20">&gt;VALUE</text>
<rectangle x1="-0.254" y1="-0.254" x2="0.254" y2="0.254" layer="51"/>
</package>
<package name="1X01_POGOPIN_HOLE_0.061_DIA">
<description>&lt;h3&gt;Pogo Pin - 0.061"&lt;/h3&gt;
&lt;p&gt;Specifications:
&lt;ul&gt;&lt;li&gt;Pin count:1&lt;/li&gt;
&lt;li&gt;Pin pitch:0.061"&lt;/li&gt;
&lt;/ul&gt;&lt;/p&gt;
&lt;p&gt;Example device(s):
&lt;ul&gt;&lt;li&gt;CONN_01&lt;/li&gt;
&lt;/ul&gt;&lt;/p&gt;</description>
<circle x="0" y="0" radius="0.635" width="0.127" layer="51"/>
<pad name="1" x="0" y="0" drill="0.9" diameter="0.8128" rot="R90" thermals="no"/>
<hole x="0" y="0" drill="1.5494"/>
<text x="-1.27" y="1.143" size="0.6096" layer="25" font="vector" ratio="20">&gt;NAME</text>
<text x="-1.27" y="-1.778" size="0.6096" layer="27" font="vector" ratio="20">&gt;VALUE</text>
</package>
<package name="1X01_POGOPIN_HOLE_0.58_DIA">
<description>&lt;h3&gt;Pogo Pin Hole - 0.58" &lt;/h3&gt;
&lt;p&gt;Specifications:
&lt;ul&gt;&lt;li&gt;Pin count:1&lt;/li&gt;
&lt;li&gt;Pin pitch:0.58"&lt;/li&gt;
&lt;/ul&gt;&lt;/p&gt;
&lt;p&gt;Example device(s):
&lt;ul&gt;&lt;li&gt;CONN_01&lt;/li&gt;
&lt;/ul&gt;&lt;/p&gt;</description>
<circle x="0" y="0" radius="0.635" width="0.127" layer="51"/>
<pad name="1" x="0" y="0" drill="0.9" diameter="0.8128" rot="R90" thermals="no"/>
<hole x="0" y="0" drill="1.4732"/>
<text x="-1.27" y="1.143" size="0.6096" layer="25" font="vector" ratio="20">&gt;NAME</text>
<text x="-1.27" y="-1.778" size="0.6096" layer="27" font="vector" ratio="20">&gt;VALUE</text>
</package>
<package name="SNAP-FEMALE">
<description>&lt;h3&gt;Sew-On Fabric Snap - Female&lt;/h3&gt;
Equivalent to size #1/0 snap. 
&lt;p&gt;Specifications:
&lt;ul&gt;&lt;li&gt;Pin count: 1&lt;/li&gt;
&lt;li&gt;Area:8mm&lt;/li&gt;
&lt;/ul&gt;&lt;/p&gt;
&lt;p&gt;Example device(s):
&lt;ul&gt;&lt;li&gt;CONN_01&lt;/li&gt;
&lt;/ul&gt;&lt;/p&gt;</description>
<pad name="1" x="0" y="0" drill="2.921" diameter="4.572"/>
<polygon width="0.254" layer="1">
<vertex x="-4.0005" y="0" curve="-89.997136"/>
<vertex x="0" y="4.0005" curve="-90.002865"/>
<vertex x="4.0005" y="0" curve="-89.997136"/>
<vertex x="0" y="-4.0005" curve="-89.997136"/>
</polygon>
<polygon width="0.3556" layer="29">
<vertex x="-4.0005" y="0" curve="-90.002865"/>
<vertex x="0" y="4.0005" curve="-90.002865"/>
<vertex x="4.0005" y="0" curve="-89.997136"/>
<vertex x="0" y="-4.0005" curve="-89.997136"/>
</polygon>
<polygon width="0.3556" layer="31">
<vertex x="-4.0005" y="0" curve="-89.997136"/>
<vertex x="0" y="4.0005" curve="-90.002865"/>
<vertex x="4.0005" y="0" curve="-89.997136"/>
<vertex x="0" y="-4.0005" curve="-89.997136"/>
</polygon>
<polygon width="0.3556" layer="41">
<vertex x="-4.0005" y="0" curve="-89.997136"/>
<vertex x="0" y="4.0005" curve="-90.002865"/>
<vertex x="4.0005" y="0" curve="-89.997136"/>
<vertex x="0" y="-4.0005" curve="-89.997136"/>
</polygon>
<text x="-1.27" y="4.318" size="0.6096" layer="25" font="vector" ratio="20">&gt;NAME</text>
<text x="-1.27" y="-4.953" size="0.6096" layer="27" font="vector" ratio="20">&gt;VALUE</text>
</package>
<package name="SNAP-MALE">
<description>&lt;h3&gt;Sew-On Fabric Snap - Male&lt;/h3&gt;
Equivalent to size #1/0 snap. 
&lt;p&gt;Specifications:
&lt;ul&gt;&lt;li&gt;Pin count: 1&lt;/li&gt;
&lt;li&gt;Area:8mm&lt;/li&gt;
&lt;/ul&gt;&lt;/p&gt;
&lt;p&gt;Example device(s):
&lt;ul&gt;&lt;li&gt;CONN_01&lt;/li&gt;
&lt;/ul&gt;&lt;/p&gt;</description>
<smd name="2" x="0" y="0" dx="7.62" dy="7.62" layer="1" roundness="100"/>
<text x="-1.524" y="4.064" size="0.6096" layer="25" font="vector" ratio="20">&gt;NAME</text>
<text x="-1.651" y="-4.826" size="0.6096" layer="27" font="vector" ratio="20">&gt;VALUE</text>
</package>
<package name="SPRING-CONNECTOR">
<description>&lt;h3&gt;Spring Connector&lt;/h3&gt;
&lt;p&gt;Specifications:
&lt;ul&gt;&lt;li&gt;Pin count: 1&lt;/li&gt;
&lt;li&gt;Area:0.25"&lt;/li&gt;
&lt;/ul&gt;&lt;/p&gt;
&lt;p&gt;Example device(s):
&lt;ul&gt;&lt;li&gt;CONN_01&lt;/li&gt;
&lt;/ul&gt;&lt;/p&gt;</description>
<smd name="P$2" x="0" y="0" dx="7.112" dy="7.112" layer="1" roundness="100"/>
<text x="-1.27" y="3.81" size="0.6096" layer="25" font="vector" ratio="20">&gt;NAME</text>
<text x="-1.524" y="-4.572" size="0.6096" layer="27" font="vector" ratio="20">&gt;VALUE</text>
</package>
<package name="1X01NS_KIT">
<description>&lt;h3&gt;Plated Through Hole - No Silk Outline Kit Version&lt;/h3&gt;
&lt;p&gt; Mask on only one side to make soldering in kits easier.
&lt;p&gt;Specifications:
&lt;ul&gt;&lt;li&gt;Pin count:1&lt;/li&gt;
&lt;li&gt;Pin pitch:0.1"&lt;/li&gt;
&lt;/ul&gt;&lt;/p&gt;
&lt;p&gt;Example device(s):
&lt;ul&gt;&lt;li&gt;CONN_01&lt;/li&gt;
&lt;/ul&gt;&lt;/p&gt;</description>
<pad name="1" x="0" y="0" drill="1.016" diameter="1.8796" rot="R90" stop="no"/>
<circle x="0" y="0" radius="0.508" width="0" layer="29"/>
<circle x="0" y="0" radius="0.9398" width="0" layer="30"/>
<text x="-1.27" y="1.143" size="0.6096" layer="25" font="vector" ratio="20">&gt;NAME</text>
<text x="-1.27" y="-1.016" size="0.6096" layer="27" font="vector" ratio="20" align="top-left">&gt;VALUE</text>
</package>
<package name="1X01_NO_SILK">
<description>&lt;h3&gt;Plated Through Hole - No Silk Outline&lt;/h3&gt;
&lt;p&gt;Specifications:
&lt;ul&gt;&lt;li&gt;Pin count:1&lt;/li&gt;
&lt;li&gt;Pin pitch:0.1"&lt;/li&gt;
&lt;/ul&gt;&lt;/p&gt;
&lt;p&gt;Example device(s):
&lt;ul&gt;&lt;li&gt;CONN_01&lt;/li&gt;
&lt;/ul&gt;&lt;/p&gt;</description>
<pad name="1" x="0" y="0" drill="1.016" diameter="1.8796" rot="R90"/>
<text x="-1.27" y="1.143" size="0.6096" layer="25" font="vector" ratio="20">&gt;NAME</text>
<text x="-1.27" y="-1.778" size="0.6096" layer="27" font="vector" ratio="20">&gt;VALUE</text>
<rectangle x1="-0.254" y1="-0.254" x2="0.254" y2="0.254" layer="51"/>
</package>
<package name="SMTSO-256-ET-0.165DIA">
<description>&lt;h3&gt;SMTSO-256-ET Flush Mount Nut&lt;/h3&gt;
.165 drill
&lt;br&gt;
Fits 4-40 Screws. 
&lt;p&gt;Specifications:
&lt;ul&gt;&lt;li&gt;Pin count: 1&lt;/li&gt;
&lt;/ul&gt;&lt;/p&gt;
&lt;p&gt;Example device(s):
&lt;ul&gt;&lt;li&gt;CONN_01&lt;/li&gt;
&lt;/ul&gt;&lt;/p&gt;</description>
<circle x="0" y="0" radius="1.016" width="0.127" layer="51"/>
<wire x1="-1.016" y1="2.286" x2="-2.286" y2="1.016" width="1.016" layer="31" curve="42.075022"/>
<wire x1="2.286" y1="1.016" x2="1.016" y2="2.286" width="1.016" layer="31" curve="42.075022"/>
<wire x1="1.016" y1="-2.286" x2="2.286" y2="-1.016" width="1.016" layer="31" curve="42.075022"/>
<wire x1="-2.286" y1="-1.016" x2="-1.016" y2="-2.286" width="1.016" layer="31" curve="42.075022"/>
<pad name="P$1" x="0" y="0" drill="4.191" diameter="6.1976"/>
<text x="-1.397" y="3.302" size="0.6096" layer="25" font="vector" ratio="20">&gt;NAME</text>
<text x="-1.524" y="-3.937" size="0.6096" layer="27" font="vector" ratio="20">&gt;VALUE</text>
</package>
</packages>
<symbols>
<symbol name="CONN_01">
<description>&lt;h3&gt;1 Pin Connection&lt;/h3&gt;</description>
<wire x1="3.81" y1="-2.54" x2="-2.54" y2="-2.54" width="0.4064" layer="94"/>
<wire x1="1.27" y1="0" x2="2.54" y2="0" width="0.6096" layer="94"/>
<wire x1="-2.54" y1="2.54" x2="-2.54" y2="-2.54" width="0.4064" layer="94"/>
<wire x1="3.81" y1="-2.54" x2="3.81" y2="2.54" width="0.4064" layer="94"/>
<wire x1="-2.54" y1="2.54" x2="3.81" y2="2.54" width="0.4064" layer="94"/>
<text x="-2.54" y="-4.826" size="1.778" layer="96" font="vector">&gt;VALUE</text>
<text x="-2.54" y="3.048" size="1.778" layer="95" font="vector">&gt;NAME</text>
<pin name="1" x="7.62" y="0" visible="off" length="middle" direction="pas" swaplevel="1" rot="R180"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="CONN_01" prefix="J" uservalue="yes">
<description>&lt;h3&gt;Single connection point. Often used as Generic Header-pin footprint for 0.1 inch spaced/style header connections&lt;/h3&gt;
&lt;p&gt;&lt;/p&gt;

&lt;p&gt;&lt;/p&gt;
On any of the 0.1 inch spaced packages, you can populate with these:
&lt;ul&gt;
&lt;li&gt;&lt;a href="https://www.sparkfun.com/products/116"&gt; Break Away Headers - Straight&lt;/a&gt; (PRT-00116)&lt;/li&gt;
&lt;li&gt;&lt;a href="https://www.sparkfun.com/products/553"&gt; Break Away Male Headers - Right Angle&lt;/a&gt; (PRT-00553)&lt;/li&gt;
&lt;li&gt;&lt;a href="https://www.sparkfun.com/products/115"&gt; Female Headers&lt;/a&gt; (PRT-00115)&lt;/li&gt;
&lt;li&gt;&lt;a href="https://www.sparkfun.com/products/117"&gt; Break Away Headers - Machine Pin&lt;/a&gt; (PRT-00117)&lt;/li&gt;
&lt;li&gt;&lt;a href="https://www.sparkfun.com/products/743"&gt; Break Away Female Headers - Swiss Machine Pin&lt;/a&gt; (PRT-00743)&lt;/li&gt;
&lt;p&gt;&lt;/p&gt;
&lt;/ul&gt;
&lt;p&gt;&lt;/p&gt;
This device is also useful as a general connection point to wire up your design to another part of your project. Our various solder wires solder well into these plated through hole pads.
&lt;ul&gt;
&lt;li&gt;&lt;a href="https://www.sparkfun.com/products/11375"&gt; Hook-Up Wire - Assortment (Stranded, 22 AWG)&lt;/a&gt; (PRT-11375)&lt;/li&gt;
&lt;li&gt;&lt;a href="https://www.sparkfun.com/products/11367"&gt; Hook-Up Wire - Assortment (Solid Core, 22 AWG)&lt;/a&gt; (PRT-11367)&lt;/li&gt;
&lt;li&gt;&lt;a href="https://www.sparkfun.com/categories/141"&gt; View the entire wire category on our website here&lt;/a&gt;&lt;/li&gt;
&lt;p&gt;&lt;/p&gt;
&lt;/ul&gt;

&lt;p&gt;&lt;/p&gt;
&lt;b&gt;Special notes:&lt;/b&gt;
&lt;p&gt; &lt;/p&gt;
SMTSO-256-ET is a "flush mount" nut for a 4-40 screw. We mostly use this on specialty testbeds; it is a nice way to connect hardware to your PCB at an adjustable hieght.
&lt;p&gt;&lt;/p&gt;
Also note, the SNAP packages are for using a snappable style connector. We sell a baggie of snaps and they are also used on two LilyPad designs:
&lt;ul&gt;
&lt;li&gt;&lt;a href="https://www.sparkfun.com/products/11347"&gt; Snap Assortment - 30 pack (male and female)&lt;/a&gt; (DEV-11347)&lt;/li&gt;
&lt;li&gt;&lt;a href="https://www.sparkfun.com/products/10941"&gt;LilyPad Arduino SimpleSnap&lt;/a&gt; (DEV-10941)&lt;/li&gt;
&lt;li&gt;&lt;a href="https://www.sparkfun.com/products/10940"&gt; LilyPad SimpleSnap Protoboard&lt;/a&gt; (DEV-10940)&lt;/li&gt;
&lt;p&gt;&lt;/p&gt;
&lt;/ul&gt;</description>
<gates>
<gate name="G$1" symbol="CONN_01" x="0" y="0"/>
</gates>
<devices>
<device name="PTH_LONGPAD" package="1X01_LONGPAD">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="" package="1X01">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="PTH_2MM" package="1X01_2MM">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="OFFSET" package="1X01_OFFSET">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="POGOPIN_HOLE_LARGE" package="1X01_POGOPIN_HOLE_0.061_DIA">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="POGOPIN_HOLE_0.58" package="1X01_POGOPIN_HOLE_0.58_DIA">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="SNAP-FEMALE" package="SNAP-FEMALE">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="SNAP-MALE" package="SNAP-MALE">
<connects>
<connect gate="G$1" pin="1" pad="2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="SPRING-CONN" package="SPRING-CONNECTOR">
<connects>
<connect gate="G$1" pin="1" pad="P$2"/>
</connects>
<technologies>
<technology name="">
<attribute name="SF_ID" value="PRT-11822" constant="no"/>
</technology>
</technologies>
</device>
<device name="PTH_NO_SILK_KIT" package="1X01NS_KIT">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="PTH_NO_SILK_YES_STOP" package="1X01_NO_SILK">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="SMTSO-256-ET-0.165DIA" package="SMTSO-256-ET-0.165DIA">
<connects>
<connect gate="G$1" pin="1" pad="P$1"/>
</connects>
<technologies>
<technology name="">
<attribute name="PROD_ID" value="HW-08694" constant="no"/>
</technology>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="WM6787CT-ND">
<packages>
<package name="WM6787CT-ND">
<smd name="P$1" x="-4.7" y="2.85" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$2" x="-4.7" y="-2.65" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$3" x="-4.2" y="2.85" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$4" x="-4.2" y="-2.65" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$5" x="-3.7" y="2.85" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$6" x="-3.7" y="-2.65" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$7" x="-3.2" y="2.85" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$8" x="-3.2" y="-2.65" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$9" x="-2.7" y="2.85" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$10" x="-2.7" y="-2.65" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$11" x="-2.2" y="2.85" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$12" x="-2.2" y="-2.65" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$13" x="-1.7" y="2.85" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$14" x="-1.7" y="-2.65" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$15" x="-1.2" y="2.85" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$16" x="-1.2" y="-2.65" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$17" x="-0.7" y="2.85" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$18" x="-0.7" y="-2.65" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$19" x="-0.2" y="2.85" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$20" x="-0.2" y="-2.65" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$21" x="0.3" y="2.85" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$22" x="0.3" y="-2.65" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$23" x="0.8" y="2.85" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$24" x="0.8" y="-2.65" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$25" x="1.3" y="2.85" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$26" x="1.3" y="-2.65" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$27" x="1.8" y="2.85" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$28" x="1.8" y="-2.65" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$29" x="2.3" y="2.85" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$30" x="2.3" y="-2.65" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$31" x="2.8" y="2.85" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$32" x="2.8" y="-2.65" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$33" x="3.3" y="2.85" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$34" x="3.3" y="-2.65" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$35" x="3.8" y="2.85" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$36" x="3.8" y="-2.65" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$37" x="4.3" y="2.85" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$38" x="4.3" y="-2.65" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$39" x="4.8" y="2.85" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$40" x="4.8" y="-2.65" dx="1.1" dy="0.3" layer="1" rot="R90"/>
<smd name="P$43" x="6" y="2.74" dx="1.32" dy="1.3" layer="1" rot="R270"/>
<smd name="P$44" x="6" y="-2.54" dx="1.32" dy="1.3" layer="1" rot="R90"/>
<smd name="P$41" x="-5.9" y="2.74" dx="1.32" dy="1.3" layer="1" rot="R90"/>
<smd name="P$42" x="-5.9" y="-2.54" dx="1.32" dy="1.3" layer="1" rot="R90"/>
<circle x="-4.7" y="4.3" radius="0.22360625" width="0.127" layer="21"/>
<text x="7.9" y="0.5" size="1.27" layer="25">&gt;Name</text>
<text x="7.9" y="-1.4" size="0.6096" layer="27">&gt;Value</text>
<wire x1="-6.9" y1="2.5" x2="-6.9" y2="-2.3" width="0.127" layer="51"/>
<wire x1="-6.9" y1="-2.3" x2="7" y2="-2.3" width="0.127" layer="51"/>
<wire x1="7" y1="-2.3" x2="7" y2="2.5" width="0.127" layer="51"/>
<wire x1="7" y1="2.5" x2="-6.9" y2="2.5" width="0.127" layer="51"/>
</package>
</packages>
<symbols>
<symbol name="WM6787CT-ND">
<pin name="2" x="-15.24" y="25.4" length="middle"/>
<pin name="4" x="-15.24" y="22.86" length="middle"/>
<pin name="6" x="-15.24" y="20.32" length="middle"/>
<pin name="8" x="-15.24" y="17.78" length="middle"/>
<pin name="10" x="-15.24" y="15.24" length="middle"/>
<pin name="12" x="-15.24" y="12.7" length="middle"/>
<pin name="14" x="-15.24" y="10.16" length="middle"/>
<pin name="16" x="-15.24" y="7.62" length="middle"/>
<pin name="18" x="-15.24" y="5.08" length="middle"/>
<pin name="20" x="-15.24" y="2.54" length="middle"/>
<pin name="22" x="-15.24" y="0" length="middle"/>
<pin name="24" x="-15.24" y="-2.54" length="middle"/>
<pin name="26" x="-15.24" y="-5.08" length="middle"/>
<pin name="28" x="-15.24" y="-7.62" length="middle"/>
<pin name="30" x="-15.24" y="-10.16" length="middle"/>
<pin name="32" x="-15.24" y="-12.7" length="middle"/>
<pin name="1" x="-15.24" y="-15.24" length="middle"/>
<pin name="3" x="-15.24" y="-17.78" length="middle"/>
<pin name="5" x="-15.24" y="-20.32" length="middle"/>
<pin name="7" x="-15.24" y="-22.86" length="middle"/>
<pin name="9" x="-15.24" y="-25.4" length="middle"/>
<pin name="11" x="-15.24" y="-27.94" length="middle"/>
<pin name="13" x="-15.24" y="-30.48" length="middle"/>
<pin name="15" x="-15.24" y="-33.02" length="middle"/>
<pin name="17" x="-15.24" y="-35.56" length="middle"/>
<pin name="19" x="15.24" y="17.78" length="middle" rot="R180"/>
<pin name="21" x="15.24" y="15.24" length="middle" rot="R180"/>
<pin name="23" x="15.24" y="12.7" length="middle" rot="R180"/>
<pin name="25" x="15.24" y="10.16" length="middle" rot="R180"/>
<pin name="27" x="15.24" y="7.62" length="middle" rot="R180"/>
<pin name="29" x="15.24" y="5.08" length="middle" rot="R180"/>
<pin name="33" x="15.24" y="2.54" length="middle" rot="R180"/>
<pin name="35" x="15.24" y="0" length="middle" rot="R180"/>
<pin name="37" x="15.24" y="-2.54" length="middle" rot="R180"/>
<pin name="38" x="15.24" y="-5.08" length="middle" rot="R180"/>
<pin name="39" x="15.24" y="-7.62" length="middle" rot="R180"/>
<pin name="40" x="15.24" y="-10.16" length="middle" rot="R180"/>
<pin name="31" x="15.24" y="-12.7" length="middle" rot="R180"/>
<pin name="34" x="15.24" y="-15.24" length="middle" rot="R180"/>
<pin name="36" x="15.24" y="-17.78" length="middle" rot="R180"/>
<pin name="41" x="15.24" y="-20.32" length="middle" rot="R180"/>
<pin name="42" x="15.24" y="-22.86" length="middle" rot="R180"/>
<pin name="43" x="15.24" y="-25.4" length="middle" rot="R180"/>
<pin name="44" x="15.24" y="-27.94" length="middle" rot="R180"/>
<wire x1="-10.16" y1="27.94" x2="-10.16" y2="-38.1" width="0.254" layer="94"/>
<wire x1="-10.16" y1="-38.1" x2="10.16" y2="-38.1" width="0.254" layer="94"/>
<wire x1="10.16" y1="-38.1" x2="10.16" y2="27.94" width="0.254" layer="94"/>
<wire x1="10.16" y1="27.94" x2="-10.16" y2="27.94" width="0.254" layer="94"/>
<text x="-10.16" y="33.02" size="1.778" layer="95">&gt;NAME</text>
<text x="-10.16" y="-43.18" size="1.778" layer="96">&gt;VALUE</text>
</symbol>
</symbols>
<devicesets>
<deviceset name="WM6787CT-ND" prefix="J" uservalue="yes">
<description>&lt;b&gt;WM6787CT-ND&lt;/b&gt; - Motherboard keyboard connector
 
&lt;p&gt;  Digikey part number WM6787CT-ND&lt;r/&gt;
&lt;p&gt; Molex part number 0543630489&lt;r/&gt;
&lt;p&gt; Technical Specifications:
&lt;ul&gt;
&lt;li&gt; 40 signal pins, odds on top, evens on bottom&lt;/li&gt;
&lt;li&gt;Pin Pitch is 0.5mm&lt;/li&gt;
&lt;li&gt;4 additional ground pins on the corners&lt;/li&gt;</description>
<gates>
<gate name="G$1" symbol="WM6787CT-ND" x="0" y="0"/>
</gates>
<devices>
<device name="" package="WM6787CT-ND">
<connects>
<connect gate="G$1" pin="1" pad="P$1"/>
<connect gate="G$1" pin="10" pad="P$10"/>
<connect gate="G$1" pin="11" pad="P$11"/>
<connect gate="G$1" pin="12" pad="P$12"/>
<connect gate="G$1" pin="13" pad="P$13"/>
<connect gate="G$1" pin="14" pad="P$14"/>
<connect gate="G$1" pin="15" pad="P$15"/>
<connect gate="G$1" pin="16" pad="P$16"/>
<connect gate="G$1" pin="17" pad="P$17"/>
<connect gate="G$1" pin="18" pad="P$18"/>
<connect gate="G$1" pin="19" pad="P$19"/>
<connect gate="G$1" pin="2" pad="P$2"/>
<connect gate="G$1" pin="20" pad="P$20"/>
<connect gate="G$1" pin="21" pad="P$21"/>
<connect gate="G$1" pin="22" pad="P$22"/>
<connect gate="G$1" pin="23" pad="P$23"/>
<connect gate="G$1" pin="24" pad="P$24"/>
<connect gate="G$1" pin="25" pad="P$25"/>
<connect gate="G$1" pin="26" pad="P$26"/>
<connect gate="G$1" pin="27" pad="P$27"/>
<connect gate="G$1" pin="28" pad="P$28"/>
<connect gate="G$1" pin="29" pad="P$29"/>
<connect gate="G$1" pin="3" pad="P$3"/>
<connect gate="G$1" pin="30" pad="P$30"/>
<connect gate="G$1" pin="31" pad="P$31"/>
<connect gate="G$1" pin="32" pad="P$32"/>
<connect gate="G$1" pin="33" pad="P$33"/>
<connect gate="G$1" pin="34" pad="P$34"/>
<connect gate="G$1" pin="35" pad="P$35"/>
<connect gate="G$1" pin="36" pad="P$36"/>
<connect gate="G$1" pin="37" pad="P$37"/>
<connect gate="G$1" pin="38" pad="P$38"/>
<connect gate="G$1" pin="39" pad="P$39"/>
<connect gate="G$1" pin="4" pad="P$4"/>
<connect gate="G$1" pin="40" pad="P$40"/>
<connect gate="G$1" pin="41" pad="P$43"/>
<connect gate="G$1" pin="42" pad="P$44"/>
<connect gate="G$1" pin="43" pad="P$41"/>
<connect gate="G$1" pin="44" pad="P$42"/>
<connect gate="G$1" pin="5" pad="P$5"/>
<connect gate="G$1" pin="6" pad="P$6"/>
<connect gate="G$1" pin="7" pad="P$7"/>
<connect gate="G$1" pin="8" pad="P$8"/>
<connect gate="G$1" pin="9" pad="P$9"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="Teensy_3p2">
<packages>
<package name="TEENSY_3P2">
<pad name="1" x="-7.62" y="-7.62" drill="0.9652"/>
<pad name="2" x="-5.08" y="-7.62" drill="0.9652"/>
<pad name="3" x="-2.54" y="-7.62" drill="0.9652"/>
<pad name="4" x="0" y="-7.62" drill="0.9652"/>
<pad name="5" x="2.54" y="-7.62" drill="0.9652"/>
<pad name="6" x="5.08" y="-7.62" drill="0.9652"/>
<pad name="7" x="7.62" y="-7.62" drill="0.9652"/>
<pad name="8" x="10.16" y="-7.62" drill="0.9652"/>
<pad name="9" x="12.7" y="-7.62" drill="0.9652"/>
<pad name="10" x="15.24" y="-7.62" drill="0.9652"/>
<pad name="11" x="17.78" y="-7.62" drill="0.9652"/>
<pad name="12" x="20.32" y="-7.62" drill="0.9652"/>
<pad name="13" x="20.32" y="7.62" drill="0.9652"/>
<pad name="14" x="17.78" y="7.62" drill="0.9652"/>
<pad name="15" x="15.24" y="7.62" drill="0.9652"/>
<pad name="16" x="12.7" y="7.62" drill="0.9652"/>
<pad name="17" x="10.16" y="7.62" drill="0.9652"/>
<pad name="18" x="7.62" y="7.62" drill="0.9652"/>
<pad name="19" x="5.08" y="7.62" drill="0.9652"/>
<pad name="20" x="2.54" y="7.62" drill="0.9652"/>
<pad name="21" x="0" y="7.62" drill="0.9652"/>
<pad name="22" x="-2.54" y="7.62" drill="0.9652"/>
<pad name="23" x="-5.08" y="7.62" drill="0.9652"/>
<pad name="0" x="-10.16" y="-7.62" drill="0.9652"/>
<pad name="3V1" x="-7.62" y="7.62" drill="0.9652"/>
<pad name="AGND" x="-10.16" y="7.62" drill="0.9652"/>
<pad name="GND1" x="-12.7" y="-7.62" drill="0.9652"/>
<pad name="VIN" x="-12.7" y="7.62" drill="0.9652"/>
<pad name="AREF" x="-5.08" y="5.08" drill="0.9652" rot="R180"/>
<pad name="A14" x="20.32" y="5.08" drill="0.9652"/>
<pad name="PGM" x="20.32" y="2.54" drill="0.9652" rot="R180"/>
<pad name="VUSB" x="-10.16" y="5.08" drill="0.9652" rot="R180"/>
<pad name="VBAT" x="20.32" y="-5.08" drill="0.9652"/>
<pad name="3V2" x="20.32" y="-2.54" drill="0.9652"/>
<wire x1="-13.97" y1="8.89" x2="21.59" y2="8.89" width="0.3048" layer="21"/>
<wire x1="21.59" y1="-8.89" x2="-13.97" y2="-8.89" width="0.3048" layer="21"/>
<wire x1="21.59" y1="8.89" x2="21.59" y2="-8.89" width="0.3048" layer="21"/>
<wire x1="-13.97" y1="8.89" x2="-13.97" y2="3.81" width="0.3048" layer="21"/>
<wire x1="-13.97" y1="-8.89" x2="-13.97" y2="-3.81" width="0.3048" layer="21"/>
<pad name="A10" x="-2.54" y="5.08" drill="0.9652" rot="R180"/>
<pad name="GND2" x="20.32" y="0" drill="0.9652" rot="R180"/>
<pad name="A11" x="0" y="5.08" drill="0.9652" rot="R180"/>
<text x="0" y="-2.54" size="1.27" layer="21">TEENSY 3.2</text>
<wire x1="-15.24" y1="3.81" x2="-15.24" y2="-3.81" width="0.3048" layer="21"/>
<wire x1="-8.89" y1="-3.81" x2="-8.89" y2="-3.175" width="0.3048" layer="21"/>
<wire x1="-8.89" y1="-3.175" x2="-8.89" y2="-2.2225" width="0.3048" layer="21"/>
<wire x1="-8.89" y1="-2.2225" x2="-8.89" y2="-1.905" width="0.3048" layer="21"/>
<wire x1="-8.89" y1="-1.905" x2="-8.89" y2="-1.27" width="0.3048" layer="21"/>
<wire x1="-8.89" y1="-1.27" x2="-8.89" y2="-0.635" width="0.3048" layer="21"/>
<wire x1="-8.89" y1="-0.635" x2="-8.89" y2="0" width="0.3048" layer="21"/>
<wire x1="-8.89" y1="0" x2="-8.89" y2="0.635" width="0.3048" layer="21"/>
<wire x1="-8.89" y1="0.635" x2="-8.89" y2="1.27" width="0.3048" layer="21"/>
<wire x1="-8.89" y1="1.27" x2="-8.89" y2="1.905" width="0.3048" layer="21"/>
<wire x1="-8.89" y1="1.905" x2="-8.89" y2="2.2225" width="0.3048" layer="21"/>
<wire x1="-8.89" y1="2.2225" x2="-8.89" y2="3.175" width="0.3048" layer="21"/>
<wire x1="-8.89" y1="3.175" x2="-8.89" y2="3.81" width="0.3048" layer="21"/>
<wire x1="-8.89" y1="0" x2="-7.62" y2="0" width="0.127" layer="21"/>
<wire x1="-8.89" y1="0.635" x2="-7.62" y2="0.635" width="0.127" layer="21"/>
<wire x1="-8.89" y1="1.27" x2="-7.62" y2="1.27" width="0.127" layer="21"/>
<wire x1="-8.89" y1="-0.635" x2="-7.62" y2="-0.635" width="0.127" layer="21"/>
<wire x1="-8.89" y1="-1.27" x2="-7.62" y2="-1.27" width="0.127" layer="21"/>
<wire x1="-8.89" y1="1.905" x2="-7.62" y2="1.905" width="0.127" layer="21"/>
<wire x1="-7.62" y1="1.905" x2="-7.62" y2="3.175" width="0.127" layer="21"/>
<wire x1="-7.62" y1="3.175" x2="-8.89" y2="3.175" width="0.127" layer="21"/>
<wire x1="-8.89" y1="-1.905" x2="-7.62" y2="-1.905" width="0.127" layer="21"/>
<wire x1="-7.62" y1="-1.905" x2="-7.62" y2="-3.175" width="0.127" layer="21"/>
<wire x1="-7.62" y1="-3.175" x2="-8.89" y2="-3.175" width="0.127" layer="21"/>
<wire x1="-13.335" y1="-3.81" x2="-13.335" y2="-5.08" width="0.127" layer="21"/>
<wire x1="-13.335" y1="3.175" x2="-11.7475" y2="3.175" width="0.127" layer="21"/>
<wire x1="-11.7475" y1="3.175" x2="-11.43" y2="2.8575" width="0.127" layer="21" curve="-90"/>
<wire x1="-11.43" y1="2.8575" x2="-11.43" y2="2.2225" width="0.127" layer="21"/>
<wire x1="-11.43" y1="2.2225" x2="-11.7475" y2="1.905" width="0.127" layer="21" curve="-90"/>
<wire x1="-11.7475" y1="1.905" x2="-13.335" y2="1.905" width="0.127" layer="21"/>
<wire x1="-13.335" y1="-3.175" x2="-11.7475" y2="-3.175" width="0.127" layer="21"/>
<wire x1="-11.7475" y1="-3.175" x2="-11.43" y2="-2.8575" width="0.127" layer="21" curve="90"/>
<wire x1="-11.43" y1="-2.8575" x2="-11.43" y2="-2.2225" width="0.127" layer="21"/>
<wire x1="-11.43" y1="-2.2225" x2="-11.7475" y2="-1.905" width="0.127" layer="21" curve="90"/>
<wire x1="-11.7475" y1="-1.905" x2="-13.335" y2="-1.905" width="0.127" layer="21"/>
<wire x1="-13.335" y1="3.175" x2="-13.335" y2="1.905" width="0.127" layer="21"/>
<wire x1="-13.335" y1="-1.905" x2="-13.335" y2="-3.175" width="0.127" layer="21"/>
<wire x1="-8.89" y1="1.27" x2="-10.16" y2="1.27" width="0.127" layer="21"/>
<wire x1="-10.16" y1="1.27" x2="-10.16" y2="3.175" width="0.127" layer="21"/>
<wire x1="-10.16" y1="3.175" x2="-9.694921875" y2="3.175" width="0.127" layer="21"/>
<wire x1="-9.694921875" y1="3.175" x2="-9.43074375" y2="3.033615625" width="0.127" layer="21" curve="-56.310402"/>
<wire x1="-9.43074375" y1="3.033615625" x2="-8.89" y2="2.2225" width="0.127" layer="21"/>
<wire x1="-8.89" y1="-1.27" x2="-10.16" y2="-1.27" width="0.127" layer="21"/>
<wire x1="-10.16" y1="-1.27" x2="-10.16" y2="-3.175" width="0.127" layer="21"/>
<wire x1="-10.16" y1="-3.175" x2="-9.694921875" y2="-3.175" width="0.127" layer="21"/>
<wire x1="-9.694921875" y1="-3.175" x2="-9.43074375" y2="-3.033615625" width="0.127" layer="21" curve="56.310402"/>
<wire x1="-9.43074375" y1="-3.033615625" x2="-8.89" y2="-2.2225" width="0.127" layer="21"/>
<wire x1="-12.065" y1="-3.81" x2="-12.065" y2="-5.08" width="0.127" layer="21"/>
<wire x1="-12.065" y1="-5.08" x2="-13.335" y2="-5.08" width="0.127" layer="21"/>
<wire x1="-13.335" y1="3.81" x2="-13.335" y2="5.08" width="0.127" layer="21"/>
<wire x1="-12.065" y1="3.81" x2="-12.065" y2="5.08" width="0.127" layer="21"/>
<wire x1="-12.065" y1="5.08" x2="-13.335" y2="5.08" width="0.127" layer="21"/>
<wire x1="-14.605" y1="-3.81" x2="-8.89" y2="-3.81" width="0.3048" layer="21"/>
<wire x1="-14.605" y1="3.81" x2="-8.89" y2="3.81" width="0.3048" layer="21"/>
<pad name="RST" x="-6.35" y="0" drill="0.9652" rot="R180"/>
<pad name="29" x="17.78" y="2.54" drill="0.9652" rot="R180"/>
<pad name="30" x="15.24" y="2.54" drill="0.9652" rot="R180"/>
<pad name="31" x="12.7" y="2.54" drill="0.9652" rot="R180"/>
<pad name="32" x="10.16" y="2.54" drill="0.9652" rot="R180"/>
<pad name="33" x="7.62" y="2.54" drill="0.9652" rot="R180"/>
<pad name="3V3" x="5.08" y="2.54" drill="0.9652" rot="R180"/>
<pad name="A13" x="2.54" y="2.54" drill="0.9652" rot="R180"/>
<pad name="28" x="17.78" y="0" drill="0.9652" rot="R180"/>
<pad name="27" x="15.24" y="0" drill="0.9652" rot="R180"/>
<pad name="26" x="12.7" y="0" drill="0.9652" rot="R180"/>
<pad name="25" x="10.16" y="0" drill="0.9652" rot="R180"/>
<pad name="24" x="7.62" y="0" drill="0.9652" rot="R180"/>
<pad name="GND3" x="5.08" y="0" drill="0.9652" rot="R180"/>
<pad name="A12" x="2.54" y="0" drill="0.9652" rot="R180"/>
</package>
</packages>
<symbols>
<symbol name="TEENSY_3P2">
<pin name="0" x="-12.7" y="40.64" length="middle"/>
<pin name="1" x="-12.7" y="38.1" length="middle"/>
<pin name="2" x="-12.7" y="35.56" length="middle"/>
<pin name="3" x="-12.7" y="33.02" length="middle"/>
<pin name="4" x="-12.7" y="30.48" length="middle"/>
<pin name="5" x="-12.7" y="27.94" length="middle"/>
<pin name="6" x="-12.7" y="25.4" length="middle"/>
<pin name="7" x="-12.7" y="22.86" length="middle"/>
<pin name="8" x="-12.7" y="20.32" length="middle"/>
<pin name="9" x="-12.7" y="17.78" length="middle"/>
<pin name="10" x="-12.7" y="15.24" length="middle"/>
<pin name="11" x="-12.7" y="12.7" length="middle"/>
<pin name="12" x="-12.7" y="10.16" length="middle"/>
<pin name="13" x="-12.7" y="7.62" length="middle"/>
<pin name="14" x="-12.7" y="5.08" length="middle"/>
<pin name="15" x="-12.7" y="2.54" length="middle"/>
<pin name="16" x="-12.7" y="0" length="middle"/>
<pin name="17" x="-12.7" y="-2.54" length="middle"/>
<pin name="18" x="-12.7" y="-5.08" length="middle"/>
<pin name="19" x="-12.7" y="-7.62" length="middle"/>
<pin name="20" x="-12.7" y="-10.16" length="middle"/>
<pin name="21" x="-12.7" y="-12.7" length="middle"/>
<pin name="22" x="-12.7" y="-15.24" length="middle"/>
<pin name="23" x="-12.7" y="-17.78" length="middle"/>
<pin name="24" x="-12.7" y="-20.32" length="middle"/>
<pin name="25" x="-12.7" y="-22.86" length="middle"/>
<pin name="26" x="-12.7" y="-25.4" length="middle"/>
<pin name="27" x="-12.7" y="-27.94" length="middle"/>
<pin name="28" x="-12.7" y="-30.48" length="middle"/>
<pin name="29" x="-12.7" y="-33.02" length="middle"/>
<pin name="30" x="-12.7" y="-35.56" length="middle"/>
<pin name="31" x="-12.7" y="-38.1" length="middle"/>
<pin name="32" x="-12.7" y="-40.64" length="middle"/>
<pin name="33" x="-12.7" y="-43.18" length="middle"/>
<pin name="GND@3" x="15.24" y="-22.86" length="middle" direction="pwr" rot="R180"/>
<pin name="GND@2" x="15.24" y="-20.32" length="middle" direction="pwr" rot="R180"/>
<pin name="GND@1" x="15.24" y="-17.78" length="middle" direction="pwr" rot="R180"/>
<pin name="AGND" x="15.24" y="-15.24" length="middle" direction="pwr" rot="R180"/>
<pin name="AREF" x="15.24" y="-12.7" length="middle" direction="pwr" rot="R180"/>
<pin name="VIN" x="15.24" y="-10.16" length="middle" direction="pwr" rot="R180"/>
<pin name="VUSB" x="15.24" y="-7.62" length="middle" direction="pwr" rot="R180"/>
<pin name="3V@1" x="15.24" y="-5.08" length="middle" direction="pwr" rot="R180"/>
<pin name="3V@2" x="15.24" y="-2.54" length="middle" direction="pwr" rot="R180"/>
<pin name="3V@3" x="15.24" y="0" length="middle" direction="pwr" rot="R180"/>
<pin name="VBAT" x="15.24" y="2.54" length="middle" direction="pwr" rot="R180"/>
<pin name="PGM" x="15.24" y="5.08" length="middle" rot="R180"/>
<pin name="RST" x="15.24" y="7.62" length="middle" rot="R180"/>
<pin name="A14" x="15.24" y="10.16" length="middle" rot="R180"/>
<pin name="A13" x="15.24" y="12.7" length="middle" rot="R180"/>
<pin name="A12" x="15.24" y="15.24" length="middle" rot="R180"/>
<pin name="A11" x="15.24" y="17.78" length="middle" rot="R180"/>
<pin name="A10" x="15.24" y="20.32" length="middle" rot="R180"/>
<wire x1="-7.62" y1="45.72" x2="-7.62" y2="-48.26" width="0.254" layer="94"/>
<wire x1="-7.62" y1="-48.26" x2="10.16" y2="-48.26" width="0.254" layer="94"/>
<wire x1="10.16" y1="-48.26" x2="10.16" y2="45.72" width="0.254" layer="94"/>
<wire x1="10.16" y1="45.72" x2="-7.62" y2="45.72" width="0.254" layer="94"/>
<text x="-7.62" y="48.26" size="1.27" layer="95">&gt;NAME</text>
<text x="-7.62" y="-50.8" size="1.27" layer="96">&gt;VALUE</text>
</symbol>
</symbols>
<devicesets>
<deviceset name="TEENSY_3P2" prefix="U" uservalue="yes">
<description>&lt;b&gt;Teensy 3.2&lt;/b&gt; - Microcontroller

&lt;p&gt;Technical Specifications:
&lt;ul&gt;
&lt;li&gt;VCC: 3.3V signals, 5V tolerant&lt;/li&gt;
&lt;li&gt;72 MHz Cortex-M4&lt;/li&gt;
&lt;li&gt;32 bit ARM Processor&lt;/li&gt;
&lt;li&gt;100% Compatible with Teensy 3.1&lt;/li&gt;
&lt;/ul&gt;
&lt;/p&gt;

&lt;p&gt;pjrc.com/store/teensy32.html &lt;BR/&gt;</description>
<gates>
<gate name="G$1" symbol="TEENSY_3P2" x="0" y="0"/>
</gates>
<devices>
<device name="" package="TEENSY_3P2">
<connects>
<connect gate="G$1" pin="0" pad="0"/>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="10" pad="10"/>
<connect gate="G$1" pin="11" pad="11"/>
<connect gate="G$1" pin="12" pad="12"/>
<connect gate="G$1" pin="13" pad="13"/>
<connect gate="G$1" pin="14" pad="14"/>
<connect gate="G$1" pin="15" pad="15"/>
<connect gate="G$1" pin="16" pad="16"/>
<connect gate="G$1" pin="17" pad="17"/>
<connect gate="G$1" pin="18" pad="18"/>
<connect gate="G$1" pin="19" pad="19"/>
<connect gate="G$1" pin="2" pad="2"/>
<connect gate="G$1" pin="20" pad="20"/>
<connect gate="G$1" pin="21" pad="21"/>
<connect gate="G$1" pin="22" pad="22"/>
<connect gate="G$1" pin="23" pad="23"/>
<connect gate="G$1" pin="24" pad="24"/>
<connect gate="G$1" pin="25" pad="25"/>
<connect gate="G$1" pin="26" pad="26"/>
<connect gate="G$1" pin="27" pad="27"/>
<connect gate="G$1" pin="28" pad="28"/>
<connect gate="G$1" pin="29" pad="29"/>
<connect gate="G$1" pin="3" pad="3"/>
<connect gate="G$1" pin="30" pad="30"/>
<connect gate="G$1" pin="31" pad="31"/>
<connect gate="G$1" pin="32" pad="32"/>
<connect gate="G$1" pin="33" pad="33"/>
<connect gate="G$1" pin="3V@1" pad="3V1"/>
<connect gate="G$1" pin="3V@2" pad="3V2"/>
<connect gate="G$1" pin="3V@3" pad="3V3"/>
<connect gate="G$1" pin="4" pad="4"/>
<connect gate="G$1" pin="5" pad="5"/>
<connect gate="G$1" pin="6" pad="6"/>
<connect gate="G$1" pin="7" pad="7"/>
<connect gate="G$1" pin="8" pad="8"/>
<connect gate="G$1" pin="9" pad="9"/>
<connect gate="G$1" pin="A10" pad="A10"/>
<connect gate="G$1" pin="A11" pad="A11"/>
<connect gate="G$1" pin="A12" pad="A12"/>
<connect gate="G$1" pin="A13" pad="A13"/>
<connect gate="G$1" pin="A14" pad="A14"/>
<connect gate="G$1" pin="AGND" pad="AGND"/>
<connect gate="G$1" pin="AREF" pad="AREF"/>
<connect gate="G$1" pin="GND@1" pad="GND1"/>
<connect gate="G$1" pin="GND@2" pad="GND2"/>
<connect gate="G$1" pin="GND@3" pad="GND3"/>
<connect gate="G$1" pin="PGM" pad="PGM"/>
<connect gate="G$1" pin="RST" pad="RST"/>
<connect gate="G$1" pin="VBAT" pad="VBAT"/>
<connect gate="G$1" pin="VIN" pad="VIN"/>
<connect gate="G$1" pin="VUSB" pad="VUSB"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
</libraries>
<attributes>
</attributes>
<variantdefs>
</variantdefs>
<classes>
<class number="0" name="default" width="0" drill="0">
</class>
</classes>
<parts>
<part name="J1" library="SparkFun-Connectors" deviceset="CONN_01" device=""/>
<part name="J2" library="SparkFun-Connectors" deviceset="CONN_01" device=""/>
<part name="J3" library="SparkFun-Connectors" deviceset="CONN_01" device=""/>
<part name="J4" library="SparkFun-Connectors" deviceset="CONN_01" device=""/>
<part name="J5" library="SparkFun-Connectors" deviceset="CONN_01" device=""/>
<part name="J6" library="SparkFun-Connectors" deviceset="CONN_01" device=""/>
<part name="J7" library="SparkFun-Connectors" deviceset="CONN_01" device=""/>
<part name="J8" library="SparkFun-Connectors" deviceset="CONN_01" device=""/>
<part name="J9" library="SparkFun-Connectors" deviceset="CONN_01" device=""/>
<part name="J10" library="SparkFun-Connectors" deviceset="CONN_01" device=""/>
<part name="J11" library="SparkFun-Connectors" deviceset="CONN_01" device=""/>
<part name="J12" library="SparkFun-Connectors" deviceset="CONN_01" device=""/>
<part name="J13" library="SparkFun-Connectors" deviceset="CONN_01" device=""/>
<part name="J14" library="SparkFun-Connectors" deviceset="CONN_01" device=""/>
<part name="J15" library="SparkFun-Connectors" deviceset="CONN_01" device=""/>
<part name="J16" library="WM6787CT-ND" deviceset="WM6787CT-ND" device=""/>
<part name="U1" library="Teensy_3p2" deviceset="TEENSY_3P2" device=""/>
</parts>
<sheets>
<sheet>
<plain>
</plain>
<instances>
<instance part="J1" gate="G$1" x="91.44" y="152.4"/>
<instance part="J2" gate="G$1" x="91.44" y="142.24"/>
<instance part="J3" gate="G$1" x="76.2" y="152.4"/>
<instance part="J4" gate="G$1" x="76.2" y="142.24"/>
<instance part="J5" gate="G$1" x="60.96" y="152.4"/>
<instance part="J6" gate="G$1" x="60.96" y="142.24"/>
<instance part="J7" gate="G$1" x="60.96" y="165.1"/>
<instance part="J8" gate="G$1" x="76.2" y="165.1"/>
<instance part="J9" gate="G$1" x="91.44" y="165.1"/>
<instance part="J10" gate="G$1" x="91.44" y="129.54"/>
<instance part="J11" gate="G$1" x="76.2" y="129.54"/>
<instance part="J12" gate="G$1" x="60.96" y="129.54"/>
<instance part="J13" gate="G$1" x="60.96" y="116.84"/>
<instance part="J14" gate="G$1" x="76.2" y="116.84"/>
<instance part="J15" gate="G$1" x="91.44" y="116.84"/>
<instance part="J16" gate="G$1" x="154.94" y="-2.54"/>
<instance part="U1" gate="G$1" x="-17.78" y="-7.62" rot="R180"/>
</instances>
<busses>
</busses>
<nets>
</nets>
</sheet>
</sheets>
</schematic>
</drawing>
</eagle>
