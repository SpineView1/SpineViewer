!function(e) {
    "use strict";

    var n = e.document;

    var cookiejar = {
        get: function(name) {
            return decodeURIComponent(n.cookie.replace(new RegExp("(?:(?:^|.*;)\\s*" + encodeURIComponent(name).replace(/[\-\.\+\*]/g, "\\$&") + "\\s*\\=\\s*([^;]*).*$)|^.*$"), "$1")) || null;
        },
        set: function(name, value, expires, path, domain, secure) {
            if (!name || /^(?:expires|max\-age|path|domain|secure)$/i.test(name)) return false;
            var strExpires = "";
            if (expires) {
                switch (expires.constructor) {
                    case Number:
                        strExpires = expires === Infinity ? "; expires=Fri, 31 Dec 9999 23:59:59 GMT" : "; max-age=" + expires;
                        break;
                    case String:
                        strExpires = "; expires=" + expires;
                        break;
                    case Date:
                        strExpires = "; expires=" + expires.toUTCString();
                        break;
                }
            }
            n.cookie = encodeURIComponent(name) + "=" + encodeURIComponent(value) + strExpires + (domain ? "; domain=" + domain : "") + (path ? "; path=" + path : "") + (secure ? "; secure" : "");
            return true;
        },
        has: function(name) {
            return new RegExp("(?:^|;\\s*)" + encodeURIComponent(name).replace(/[\-\.\+\*]/g, "\\$&") + "\\s*\\=").test(n.cookie);
        },
        remove: function(name, path, domain) {
            return name && this.has(name) ? (n.cookie = encodeURIComponent(name) + "=; expires=Thu, 01 Jan 1970 00:00:00 GMT" + (domain ? "; domain=" + domain : "") + (path ? "; path=" + path : ""), true) : false;
        }
    };

    function Cookiebanner(options) {
        this.init(options);
    }

    Cookiebanner.prototype = {
        inserted: false,
        closed: false,
        test_mode: false,
        emailForm: null,

        init: function(options) {
            this.emailForm = this.createEmailForm();

            var message = "We use cookies to enhance your experience. By continuing to visit this site you agree to our use of cookies.";
            var linkmsg = "Learn more";

            this.default_options = {
                cookie: "cookiebanner-accepted",
                closeText: "✖",
                cookiePath: "/",
                debug: false,
                expires: Infinity,
                zindex: 255,
                mask: false,
                maskOpacity: 0.5,
                maskBackground: "#000",
                height: "auto",
                minHeight: "21px",
                bg: "#000",
                fg: "#ddd",
                link: "#aaa",
                position: "bottom",
                message: message,
                linkmsg: linkmsg,
                moreinfo: "http://aboutcookies.org",
                effect: null,
                instance: "cbinstance"
            };

            this.options = this.default_options;
            if (options) {
                this.options = this.mergeOptions(this.options, options);
            }

            this.options.zindex = parseInt(this.options.zindex, 10);
            this.options.mask = this.str2bool(this.options.mask);

            if (typeof this.options.expires === "string" && typeof e[this.options.expires] === "function") {
                this.options.expires = ethis.options.expires;
            }
            if (typeof this.options.expires === "function") {
                this.options.expires = this.options.expires();
            }
    
            var script_el = this.getScriptElement();
            if (script_el) {
                var data_attribs = this.getDataAttributes(script_el);
                this.options = this.mergeOptions(this.options, data_attribs);
            }
    
            this.run();
        },
    
        log: function() {
            if (typeof console !== "undefined") {
                console.log.apply(console, arguments);
            }
        },
    
        run: function() {
            if (!this.agreed()) {
                var self = this;
                document.getElementById("email-form").addEventListener("submit", function(event) {
                    event.preventDefault();
                    var email = document.getElementById("email-input").value;
                    cookiejar.set("email", email, Infinity);
                    setTimeout(function() {
                        self.agreeAndClose();
                    }, 500);
                });
            }
        },
    
        createEmailForm: function() {
            var emailForm = document.createElement("div");
            emailForm.id = "email-popup";
            emailForm.style.display = "none";
            emailForm.innerHTML = '<div id="email-container"><form id="email-form"><label for="email-input">Please enter your email:</label><input type="email" id="email-input" name="email"><button type="submit">Submit</button></form></div>';
            var cookiebanner = document.querySelector(".cookiebanner");
            cookiebanner.parentNode.insertBefore(emailForm, cookiebanner.nextSibling);
            return document.getElementById("email-form");
        },
    
        buildViewportMask: function() {
            var mask = null;
            if (this.options.mask === true) {
                var maskOpacity = this.options.maskOpacity;
                var maskBackground = this.options.maskBackground;
                var maskHTML = '<div id="cookiebanner-mask" style="position:fixed;top:0;left:0;width:100%;height:100%;background:' + maskBackground + ';zoom:1;filter:alpha(opacity=' + 100 * maskOpacity + ');opacity:' + maskOpacity + ';z-index:' + this.options.zindex + ';"></div>';
                var tempDiv = n.createElement("div");
                tempDiv.innerHTML = maskHTML;
                mask = tempDiv.firstChild;
            }
            return mask;
        },
    
        agree: function() {
            return cookiejar.set(this.options.cookie, 1, this.options.expires, this.options.cookiePath);
        },
    
        agreed: function() {
            return cookiejar.has(this.options.cookie);
        },
    
        close: function() {
            if (this.inserted && !this.closed) {
                if (this.element) {
                    n.body.removeChild(this.element);
                }
                if (this.element_mask) {
                    n.body.removeChild(this.element_mask);
                }
                this.closed = true;
            }
        },
    
        agreeAndClose: function() {
            if (this.agree()) {
                this.close();
            }
        },
    
        mergeOptions: function(defaults, options) {
            var merged = {};
            for (var prop in defaults) {
                if (defaults.hasOwnProperty(prop)) {
                    merged[prop] = options.hasOwnProperty(prop) ? options[prop] : defaults[prop];
                }
            }
            return merged;
        },
    
        getDataAttributes: function(el) {
            var data = {};
            if (el.hasOwnProperty("dataset")) {
                data = el.dataset;
            } else {
                var attrs = el.attributes;
                for (var i in attrs) {
                    if (attrs.hasOwnProperty(i)) {
                        var attr = attrs[i];
                        if (/^data-/.test(attr.name)) {
                            var camelizeName = this.camelize(attr.name.substr(5));
                            data[camelizeName] = attr.value;
                        }
                    }
                }
            }
            return data;
        },
        
        camelize: function(str) {
            var hyphen = "-";
            var hyphenRegex = new RegExp(hyphen + "([a-z])", "g");
            return str.replace(hyphenRegex, function(match, letter) {
                return letter.toUpperCase();
            });
        },
        
        getScriptElement: function() {
            var scripts = n.getElementsByTagName("script");
            for (var i = 0; i < scripts.length; i++) {
                if (scripts[i].id === "cookiebanner") {
                    return scripts[i];
                }
            }
            return null;
        }
        
        var instance = new Cookiebanner();
        
        t(e, function() {
            instance.run();
        });
        
