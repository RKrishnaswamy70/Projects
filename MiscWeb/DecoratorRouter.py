# General form of composed decorators. 
# Suppose you have:
#   @dec3(x1,x2)
#   @dec2
#   @dec1(y1,y2,y3)
#   def foo(a1,a2):
#       ...
# This is mathematically equivalent to the following where
# we use 'o' as the function composition operator:
#   foo = dec3(x1,x2) o dec2() o dec1(y1,y2,y3) (foo)
# Programmatically, this is:
#   def foo(a1,a2):
#       ...
#   dec3(x1,x2) 
#     (dec2()
#       (dec1(y1,y2,y3)
#          (foo)))
#

# In this example, we will use decorators to register router
# functions.  Suppose app is an instance of TinyRouter.  Then,
# declaring a function with the @app.route decorator will register
# the function within the app instannce.
class TinyRouter:
    def __init__(self):
        # The registry: maps URL paths to functions
        self.routes = {}

    def route(self, path):
        # Decorator factory that takes the URL path.
        # This decorator records 'func' in the 'routes' map.
        # It then just returns the 'func' itself.
        #
        # Hence, in a call
        #     xxxfunc = app.decorator(xxxfunc)
        # the xxxfunc is *not* changed.  The only thing that
        # is done is that the xxxfunc is automatically 
        # registered in the 'routes' map.
        # 
        # Thus a declaration
        #     @app.route("/a/b")
        #     def xxxfunc():
        #        ...
        # will automatically register xxxfunc() in the 'routes'
        # map without any special action by the client of TinyRouter.
        def decorator(func):
            print(f"Decorator called for {path}")
            # Register the function to the path
            self.routes[path] = func
            # Return function unchanged (function registration pattern)
            return func
        return decorator

    def handle_request(self, path):
        # Simulates an incoming request being routed.
        handler = self.routes.get(path)
        if handler:
            return handler()
        return "404 Not Found"



# The instance
app = TinyRouter()

# The decorator:
#    @app.route("/")
#    def index():
#        return "Welcome to the Home Page!"
# is the same as
#    def index():
#        return "Welcome to the Home Page!"
#    index = app.route("/")(index)
# Thus the inner decorator() function is called once
# at this declaration.  That call registers index in the
# "app.routes" map.  and returns the same function.  Thus
# in this case, the index function is *not* changed! 
@app.route("/")
def index():
    return "Welcome to the Home Page!"

# Here a decorator route-function is called without the @-notation
def atless():
    return "Welcome to the At-Less Page!"
atless = app.route("/atless")(atless)

@app.route("/about")
def about():
    return "Welcome to the About Page!"

# Now we will have nested decorators
@app.route("/2")
@app.route("/1")
def nested():
    return "Welcome to the Nested Page!"
# This is the same as calling the following after declaring nested():
# Mathematically:
#   nested = app.route("/2") o app.route("/1") (nested)
# Programmatically:
#   nested = app.route("/2")(app.route("/1")(nested))

# Now the routes are registered.  Hence, we can simulate routing by
# simply calling app.handle_request(<route>).  This will lookup the
# app.routes map, and call the registered function for that route.
print(app.handle_request("/"))       # Output: Welcome to the Home Page!
print(app.handle_request("/atless")) # Output: Welcome to the At-Less Page!
print(app.handle_request("/about"))  # Output: Welcome to the About Page!
print(app.handle_request("/2"))      # Output: Welcome to the Nested Page!




