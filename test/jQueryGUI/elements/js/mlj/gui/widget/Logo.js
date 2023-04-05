/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

(function (component) {

    MLJ.gui.widget.Logo = function () {

        var LOGO_WIDTH = 96;
        var LOGO_HEIGHT = 821 * LOGO_WIDTH / 1023;
        var insets = 10;
        var _PiP;

        this._make = function () {//build function                 
            _PiP = new component.PiP();

            var $logo = $('<img id="logo" src="../../../img/vcglogo200609_1024px.png">');

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
                $('body').css('background-color', 'white');
            });

            return _PiP.$;
        };
    };
    
    MLJ.extend(MLJ.gui.widget.Widget, MLJ.gui.widget.Logo);

    //Install widget
    MLJ.gui.installWidget("Logo", new MLJ.gui.widget.Logo());

})(MLJ.gui.component);
