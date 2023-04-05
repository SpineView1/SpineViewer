/**
 * MLJLib
 * MeshLabJS Library
 * 
 * Copyright(C) 2015
 * Paolo Cignoni 
 * Visual Computing Lab
 * ISTI - CNR
 * 
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free Software 
 * Foundation; either version 2 of the License, or (at your option) any later 
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE. See theGNU General Public License 
 * (http://www.gnu.org/licenses/gpl.txt) for more details.
 * 
 */

/**
 * @file Defines and installs the Logo widget
 * @author Stefano Gabriele
 */
(function (component) {
    
    /**         
     * @class Create a new Logo widget
     * @augments  MLJ.gui.Widget
     * @private
     * @memberOf MLJ.gui.widget
     * @author Stefano Gabriele 
     */
    var _Logo = function () {

        var LOGO_WIDTH = 96;
        var LOGO_HEIGHT = 821 * LOGO_WIDTH / 1023;
        var insets = 10;
        var _PiP;
        var _dialog = new MLJ.gui.component.Dialog(
                {title:"filter meshes according to different spinopelvic parameter values and categories",draggable: true, width: 1500, modal: true, resizable: false});
        
        this._make = function () {                 
            _PiP = new component.PiP();

            var $logo = $('<img id="logo" src="https://cdn-icons-png.flaticon.com/512/2080/2080946.png">');

            

            _dialog.appendContent(
                '<div class="airtable-embed">'
                + '<iframe title="SpineView" width="1495px" height="800px" src="https://app.powerbi.com/view?r=eyJrIjoiMjZhNGUzYTctOGUwZS00OGNmLWI2MDYtNjQzN2UxYTQ0Y2JkIiwidCI6IjQyZTFiNDJmLWRlNzQtNDI5MC05ZDNhLTExNDkyNTUzMTM1ZSIsImMiOjl9&embedImagePlaceholder=true&pageName=ReportSection" frameborder="0" allowFullScreen="true"></iframe>'
                + '</div>' + MLJ.core.plugin.Manager.getFilterPlugins().size() + " filters."
                );


                
            $logo.load(function () {
                _PiP.appendContent(this);
                $(this).width(LOGO_WIDTH);
            });

            $logo.css('cursor', 'pointer');

            $(document).ready(function () {
                var x = $('#mlj-tools-pane').outerWidth() + insets;
                var y = $(window).height() - LOGO_HEIGHT - insets;
                _PiP.setX(x);
                _PiP.setY(y);
            });

            $(window).resize(function () {
                var $tp = $('#mlj-tools-pane');
                var newX = $tp.outerWidth() + $tp.offset().left + insets;
                var newY = $(window).height() - LOGO_HEIGHT - insets;
                _PiP.setX(newX);
                _PiP.setY(newY);
            });

            $logo.click(function () {               
                  _dialog.show();
            });

            return _PiP.$;
        };
    };

    MLJ.extend(MLJ.gui.Widget, _Logo);

    //Install widget
    MLJ.gui.installWidget("Logo", new _Logo());

})(MLJ.gui.component);


