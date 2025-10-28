# users/utils.py
from suitcases.models import Suitcase

def calculate_jaccard_similarity(user_categories, suitcase_categories):
    """
    Calculate Jaccard similarity between user selected categories and suitcase categories. (0-1 float)
    """
    # make sets
    user_set = set(user_categories)
    suitcase_set = set(suitcase_categories)

    # calculate
    if not user_set and not suitcase_set:
        return 0.0
    elif not user_set or not suitcase_set:
        return 0.0
    
    intersection = len(user_set.intersection(suitcase_set))
    union = len(user_set.union(suitcase_set))
    return intersection / union

def get_recommended_suitcases(user_categories, max_recommendations=3):
    """ Get the recommended suitcases based on jaccard similarity """
    available_suitcases = Suitcase.objects.filter(rented=False).prefetch_related('categories')
    scored_suitcases = []
    for suitcase in available_suitcases:
        suitcase_categories = suitcase.getCategories()
        score = calculate_jaccard_similarity(user_categories, suitcase_categories)

        scored_suitcases.append({'suitcase': suitcase, 'score': score, 'categories': suitcase_categories})
    # Sort by score descending
    scored_suitcases.sort(key=lambda x: x['score'], reverse=True)
    return scored_suitcases[:max_recommendations]